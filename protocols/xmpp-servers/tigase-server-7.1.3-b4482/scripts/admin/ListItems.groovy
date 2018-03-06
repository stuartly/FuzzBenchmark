/*
 * Tigase Jabber/XMPP Server
 * Copyright (C) 2004-2016 "Tigase, Inc." <office@tigase.com>
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
 * $Rev: $
 * Last modified by $Author: $
 * $Date: $
 */

/*
 List PubSub node items

 AS:Description: List items
 AS:CommandId: list-items
 AS:Component: pubsub
 */

package tigase.admin

import tigase.server.*
import tigase.util.*
import tigase.xmpp.*
import tigase.db.*
import tigase.xml.*
import tigase.vhosts.*
import tigase.pubsub.*
import tigase.pubsub.repository.IPubSubRepository
import tigase.pubsub.exceptions.PubSubException
import tigase.pubsub.modules.NodeCreateModule.NodeCreateHandler.NodeCreateEvent;

def NODE = "node"

IPubSubRepository pubsubRepository = component.pubsubRepository

def p = (Packet)packet
def admins = (Set)adminsSet
def stanzaFromBare = p.getStanzaFrom().getBareJID()
def isServiceAdmin = admins.contains(stanzaFromBare)

def result = null;
def node = Command.getFieldValue(p, NODE);

try {
	if (!node) {
		result = p.commandResult(Command.DataType.form);
		Command.addTitle(result, "List of PubSub node items");
		Command.addInstructions(result, "Fill out this form to retrieve list of published items for node");
		Command.addFieldValue(result, NODE, node ?: "", "text-single", "Node");
	} else {
		result = p.commandResult(Command.DataType.result);
		Command.addTitle(result, "List of PubSub node items");
		if (isServiceAdmin || component.componentConfig.isAdmin(stanzaFromBare)) {
			Command.addFieldValue(result, NODE, node, "text-single", "Node");
			def nodeConfig = pubsubRepository.getNodeConfig(p.getStanzaTo().getBareJID(), node);
			if (nodeConfig == null)
				throw new PubSubException(Authorization.ITEM_NOT_FOUND, "Node " + node + " was not found");
			def items = pubsubRepository.getNodeItems(p.getStanzaTo().getBareJID(), node);
			def itemsIds = items.getItemsIds() ?: [];
			Command.addFieldMultiValue(result, "items", itemsIds as List);
			result.getElement().getChild('command').getChild('x').getChildren().find { e -> e.getAttribute("var") == "items" }?.setAttribute("label", "Items");
		} else {
			throw new PubSubException(Authorization.FORBIDDEN, "You do not have enough " +
				"permissions to list items published to a node.");
		}
	}
} catch (PubSubException ex) {
	Command.addTextField(result, "Error", ex.getMessage())
	if (ex.getErrorCondition()) {
		def error = ex.getErrorCondition();
		Element errorEl = new Element("error");
		errorEl.setAttribute("type", error.getErrorType());
		Element conditionEl = new Element(error.getCondition(), ex.getMessage());
		conditionEl.setXMLNS(Packet.ERROR_NS);
		errorEl.addChild(conditionEl);
		Element pubsubCondition = ex.pubSubErrorCondition?.getElement();
		if (pubsubCondition)
			errorEl.addChild(pubsubCondition);
		result.getElement().addChild(errorEl);
	}
}

return result
