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
import tigase.xml.*
import tigase.xmpp.BareJID
import tigase.xmpp.JID
import tigase.xmpp.StanzaType

/**
 * Class implements generic support for PubSub ad-hoc commands
 */
class PubSubActionsHandler extends tigase.http.rest.Handler {

    def TIMEOUT = 30 * 1000;

    def COMMAND_XMLNS = "http://jabber.org/protocol/commands";
    def DATA_XMLNS = "jabber:x:data";
    def DISCO_ITEMS_XMLNS = "http://jabber.org/protocol/disco#items";

    public PubSubActionsHandler() {
		description = [
			regex : "/{pubsub_jid}/{adhoc_command_node}",
			GET : [ info:'Retrieve PubSub adhoc command fields', 
				description: """This is simplified version of adhoc command for PubSub component, which allows for easier management of PubSub component nodes and items than default adhoc command REST API.
As part of url you need to pass PubSub component jid as {pubsub_jid} parameter and adhoc command node as {adhoc_command_node} for which you wish to retrieve list of fileds for.
Retrieved XML after filling it with proper data and replacing external XML element name from result to data may be passed as content for POST request to execute this command with passed parameters for command.
"""],
			POST : [ info:'Execute PubSub adhoc command',
				description: """This is simplified version of adhoc command for PubSub component, which allows for easier management of PubSub component nodes and items than default adhoc command REST API.
As part of url you need to pass PubSub component jid as {pubsub_jid} parameter and adhoc command node as {adhoc_command_node} for which you wish to execute.
For a content of a HTTP POST request you need to pass filled XML data retrieved using GET method with external element result name changed in data.
"""]
		];
        regex = /\/(?:([^@\/]+)@){0,1}([^@\/]+)\/([^\/]+)/
        isAsync = true
		decodeContent = false;

        execGet = { Service service, callback, localPart, domain, cmd ->
			execPost(service, callback, null, localPart, domain, cmd);            
        }

        execPost = { Service service, callback, content, localPart, domain, cmd ->
			//if (localPart) {
			//	localPart = localPart.substring(0,localPart.length()-1);
			//}
			
			content = decodeContent(content ? content.getReader().getText() : null);
			
            def compJid = BareJID.bareJIDInstance(localPart, domain);

            Element iq = new Element("iq");
			iq.setXMLNS(Iq.CLIENT_XMLNS);
            iq.setAttribute("to", localPart ? "$localPart@$domain" : domain);
            iq.setAttribute("type", "set");
            iq.setAttribute("id", UUID.randomUUID().toString())

            Element command = new Element("command");
            command.setXMLNS(COMMAND_XMLNS);
            command.setAttribute("node", cmd);
            iq.addChild(command);

            if (content && content.getName() == "data") {
				Element data = (Element) content;
				
				def elemNames = (data.getChildren() ?: []).collect { it.getName() }
				
                Element x = new Element("x");
                x.setXMLNS(DATA_XMLNS);
                x.setAttribute("type", "submit");
                command.addChild(x);

				elemNames.each { var ->
					List<Element> children = data.getChildren().findAll({ it.getName() == var});
					if (children.isEmpty())
						return;
						
					if (children.size() == 1 && children.get(0).getAttribute("prefix")) {
						Element wrap = children.get(0);
						String prefix = wrap.getName();
						
						wrap.getChildren().each { sub ->
							Element fieldEl = new Element("field");
							fieldEl.setAttribute("var", prefix + "#" + sub.getName());
							x.addChild(fieldEl);
							
							def values = sub.getChildren()?.findAll({it.getName() == "value"});
							if (values) {
								values.each { value ->
									Element valueEl = new Element("value", value.getCData());
									fieldEl.addChild(valueEl);						
								}			
							}
							else {
								Element valueEl = new Element("value", sub.getCData());
								fieldEl.addChild(valueEl);														
							}
						}
						return;
					}
					
					Element fieldEl = new Element("field");
					fieldEl.setAttribute("var", var);
					x.addChild(fieldEl);
					
					
					
					List<Element> values = children.get(0).getChildren()?.findAll { it.getName() == "value" };
					if (values != null && !values.isEmpty()) {
						values.each { value ->
							Element valueEl = new Element("value", value.getCData());
							fieldEl.addChild(valueEl);						
						}					
					}
					else {
						def elems = children.get(0).getChildren();
						if (elems == null || elems.isEmpty()) {
							Element valueEl = new Element("value", children.get(0).getCData());
							fieldEl.addChild(valueEl);
						}
						else {
							elems.join("").split("\n").each { value ->
								Element valueEl = new Element("value", XMLUtils.escape(value));
								fieldEl.addChild(valueEl);													
							}
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
                
				Element results = new Element("result");
				

                def titleEl = data.getChild("title");
                if (titleEl) {
					data.removeChild(titleEl);
					results.addChild(titleEl);
				}

                def instructionsEl = data.getChild("instructions");
                if (instructionsEl) {
					data.removeChild(instructionsEl);
					results.addChild(instructionsEl);
				}

                def noteEl = command.getChild("note");
                if (noteEl) {
					command.removeChild(noteEl);
					results.addChild(noteEl);
                }

				convertFieldElements(fieldElems, results);
				
				def reportedElems = data.getChildren().findAll({ it.getName() == "reported"});
				reportedElems.each { reportedEl ->
					def reported = new Element("reported");
					convertFieldElements(reportedEl.getChildren(), reported);
					results.addChild(reported);
				}
					
				def itemsElems = data.getChildren().findAll({ it.getName() == "item"});
				itemsElems.each { itemEl ->
					def items = new Element("item");				
					convertFieldElements(itemEl.getChildren(), items);
					results.addChild(items);
				}

                callback(results.toString())
            });
        }
    }
	
	def convertFieldElements = { fieldElems, results ->
		fieldElems.each { fieldEl ->
			def var = fieldEl.getAttribute("var");	
			def varTmp = var.split("#");					
			def elem = null;
			if (varTmp.length > 1) {
				elem = new Element(varTmp[1]);
				Element wrap = results.getChild(varTmp[0]);
				if (wrap == null) {
					wrap = new Element(varTmp[0]);
					wrap.setAttribute("prefix", "true");
					results.addChild(wrap);
				}
				wrap.addChild(elem);
			}
			else {
				elem = new Element(var);
				results.addChild(elem);
			}
			
			
			["label", "type"].each { attr ->
				if (fieldEl.getAttribute(attr)) {
					elem.setAttribute(attr, fieldEl.getAttribute(attr));
				}
			}
			
			def valueElems = fieldEl.getChildren().findAll({ it.getName() == "value" });
			if (valueElems != null) {
				valueElems.each { val ->
					elem.addChild(new Element("value", val.getCData()));							
				}
			}
			
			def optionElems = fieldEl.getChildren().findAll({ it.getName() == "option" });
			if (!optionElems.isEmpty()) {
				optionElems.each { optionEl ->
					Element option = new Element("option", optionEl.getChild("value").getCData());
					if (optionEl.getAttribute("label")) {
						option.setAttribute("label", optionEl.getAttribute("label"));
					}
					elem.addChild(option);
				}
			}
		}		
	}
	
	Element decodeContent(String input) {
		if (!input) return null;
		
		def handler = new DomBuilderHandler();
		def parser = SingletonFactory.getParserInstance();			
		
		def data = ((String) input).toCharArray();
		
		parser.parse(handler, data, 0, data.length);
		
		return handler.getParsedElements().get(0);		
	}

}