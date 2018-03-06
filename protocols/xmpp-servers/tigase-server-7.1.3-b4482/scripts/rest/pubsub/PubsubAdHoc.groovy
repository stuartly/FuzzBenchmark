package tigase.rest.pubsub

/*
 * Tigase HTTP API
 * Copyright (C) 2004-2013 "Tigase, Inc." <office@tigase.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. Look for COPYING file in the top folder.
 * If not, see http://www.gnu.org/licenses/.
 *
 * $Rev$
 * Last modified by $Author$
 * $Date$
 */
import tigase.http.rest.Service
import tigase.server.Command
import tigase.server.Iq
import tigase.server.Packet
import tigase.util.Base64
import tigase.xml.Element
import tigase.xmpp.BareJID
import tigase.xmpp.JID
import tigase.xmpp.StanzaType

/**
 * Class implements generic support for PubSub ad-hoc commands
 */
class PubSubHandler extends tigase.http.rest.Handler {

    def TIMEOUT = 30 * 1000;

    def COMMAND_XMLNS = "http://jabber.org/protocol/commands";
    def DATA_XMLNS = "jabber:x:data";
    def DISCO_ITEMS_XMLNS = "http://jabber.org/protocol/disco#items";

    public PubSubHandler() {
		description = [
			regex : "/{pubsub_jid}",
			GET : [ info:'Retrieve list of PubSub adhoc commands', 
				description: """To retrieve a list of available PubSub adhoc commands execute HTTP GET request with PubSub service jid passed as {pubsub_jid}.

Example response:
\${util.formatData([items:[[jid:'pubsub.example.com',node:'create-node',name:'Create node'],[jid:'pubsub.example.com',node:'delete-node',name:'Delete node']]])}
"""],
			POST : [ info:'Execute PubSub adhoc command',
				description: """As part of url you need to pass PubSub component jid as {pubsub_jid} parameter.
For a content of a HTTP POST request you need to pass XML or JSON in following form passing command node and list of fields and values needed to pass to PubSub component for proper adhoc command execution:
\${util.formatData([command:[node:'create-node', fields:[[var:'node',value:'princely_musings'],[var:'owner',value:'user@example.com']]]])}

As a result you will receive response in form of JSON or XML similar to following one which is just other representation of XMPP form result of adhoc command execution:
\${util.formatData([command:[jid:'pubsub.example.com',node:'create-node', fields:[[var:'node',value:'princely_musings'],[var:'owner',value:'user@example.com']]]])}
"""]
		];
		regex = /\/(?:([^@\/]+)@){0,1}([^@\/]+)/
        isAsync = true

        execGet = { Service service, callback, localPart, domain ->

            Element iq = new Element("iq");
			iq.setXMLNS(Iq.CLIENT_XMLNS);
            iq.setAttribute("to", localPart != null ? "$localPart@$domain" : domain);
//            iq.setAttribute("from", user.toString());
            iq.setAttribute("type", "get");
            iq.setAttribute("id", UUID.randomUUID().toString())

            Element query = new Element("query");
            query.setXMLNS(DISCO_ITEMS_XMLNS);
            query.setAttribute("node", COMMAND_XMLNS);//"http://jabber.org/protocol/commands")
            iq.addChild(query);

            service.sendPacket(new Iq(iq), TIMEOUT, { Packet result ->
                if (result == null || result.getType() == StanzaType.error) {
                    callback(null);
                    return;
                }

                def items = result.getElement().getChild("query", DISCO_ITEMS_XMLNS).getChildren().findAll({ it.getName() == "item" })
                items = items.collect { [jid: it.getAttribute("jid"), node: it.getAttribute("node"), name: it.getAttribute("name")] }

                def results = [items:items];
                callback(results)
            });
        }

        execPost = { Service service, callback, content, localPart, domain ->
            def compJid = BareJID.bareJIDInstance(localPart, domain);

            content = content.command;
            def node = content.node;
            if (!node) {
                callback(null);
                return;
            }

            def fields = content.fields;

            Element iq = new Element("iq");
			iq.setXMLNS(Iq.CLIENT_XMLNS);
            iq.setAttribute("to", localPart != null ? "$localPart@$domain" : domain);
            iq.setAttribute("type", "set");
            iq.setAttribute("id", UUID.randomUUID().toString())

            Element command = new Element("command");
            command.setXMLNS(COMMAND_XMLNS);
            command.setAttribute("node", node);
            iq.addChild(command);

            if (fields) {
                Element x = new Element("x");
                x.setXMLNS(DATA_XMLNS);
                x.setAttribute("type", "submit");
                command.addChild(x);

                println fields

                fields.each { field ->
                    Element fieldEl = new Element("field");
                    fieldEl.setAttribute("var", field.var);
                    x.addChild(fieldEl);

                    if (field.value) {
                        if (field.value instanceof Collection) {
                            field.value.each { val ->
                                Element valueEl = new Element("value", val);
                                fieldEl.addChild(valueEl);
                            }
                        }
                        else {
                            Element valueEl = new Element("value", field.value);
                            fieldEl.addChild(valueEl);
                        }
                    }
                }
            }

            service.sendPacket(new Iq(iq), TIMEOUT, { Packet result ->
				def error = result?.getElement().getChild("error");
                if (result == null || error != null) {
					String errorResult = error?.toString();
                    callback(errorResult);
                    return;
                }

                command = result.getElement().getChild("command", COMMAND_XMLNS);
                def data = command.getChild("x", DATA_XMLNS);
                def fieldElems = data.getChildren().findAll({ it.getName() == "field"});

                fields = [];
                def results = [jid: (localPart != null ? "$localPart@$domain" : domain), node: node, fields:fields];

                def titleEl = data.getChild("title");
                if (titleEl) results.title = titleEl.getCData();

                def instructionsEl = data.getChild("instructions");
                if (instructionsEl) results.instructions = instructionsEl.getCData();

                def noteEl = command.getChild("note");
                if (noteEl) {
                    results.note = [value:noteEl.getCData()];
                    if (noteEl.getAttribute("type")) {
                        results.note.type = noteEl.getAttribute("type");
                    }
                }

                fieldElems.each { fieldEl ->
                    def field = [var:fieldEl.getAttribute("var")];

                    if (fieldEl.getAttribute("label")) {
                        field.label = fieldEl.getAttribute("label");
                    }

                    if (fieldEl.getAttribute("type")) {
                        field.label = fieldEl.getAttribute("type");
                    }
                    fields.add(field);

                    def valueElems = fieldEl.getChildren().findAll({ it.getName() == "value" });
                    if (valueElems.size() == 1) {
                         field.value = valueElems.get(0).getCData();
                    }
                    else if (valueElems.size() > 1) {
                        field.value = [];
                        valueElems.each { valueEl ->
                            field.value.add(valueEl.getCData());
                        }
                    }

                    def optionElems = fieldEl.getChildren().findAll({ it.getName() == "option" });
                    if (!optionElems.isEmpty()) {
                        field.options = [];
                        optionElems.each { optionEl ->
                            def item = [value:optionEl.getChild("value").getCData()];
                            if (optionEl.getAttribute("label")) item.label = optionEl.getAttribute("label");
                            field.options.add(item);
                        }
                    }
                }

                callback([command:results])
            });
        }
    }

}