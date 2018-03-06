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
 Retrieve PubSub node item

 AS:Description: Retrieve item
 AS:CommandId: retrieve-item
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
def ITEM_ID = "item-id";

IPubSubRepository pubsubRepository = component.pubsubRepository

def p = (Packet)packet
def admins = (Set)adminsSet
def stanzaFromBare = p.getStanzaFrom().getBareJID()
def isServiceAdmin = admins.contains(stanzaFromBare)

def result = null;
def node = Command.getFieldValue(p, NODE);
def itemId = Command.getFieldValue(p, ITEM_ID);

try {
	if (!node || !itemId) {
		result = p.commandResult(Command.DataType.form);
		Command.addTitle(result, "Retrive PubSub node item");
		Command.addInstructions(result, "Fill out this form to retrieve published item");
		Command.addFieldValue(result, NODE, node ?: "", "text-single", "Node");
		Command.addFieldValue(result, ITEM_ID, itemId ?: "", "text-single", "Item ID");
	} else {
		result = p.commandResult(Command.DataType.result);
		Command.addTitle(result, "Retrive PubSub node item");
		if (isServiceAdmin || component.componentConfig.isAdmin(stanzaFromBare)) {
			Command.addFieldValue(result, NODE, node ?: "", "text-single", "Node");
			Command.addFieldValue(result, ITEM_ID, itemId ?: "", "text-single", "Item ID");
			def nodeConfig = pubsubRepository.getNodeConfig(p.getStanzaTo().getBareJID(), node);
			if (nodeConfig == null)
				throw new PubSubException(Authorization.ITEM_NOT_FOUND, "Node " + node + " was not found");
			def items = pubsubRepository.getNodeItems(p.getStanzaTo().getBareJID(), node);
			def item = items.getItem(itemId);
			if (item == null)
				throw new PubSubException(Authorization.ITEM_NOT_FOUND, "Item " + itemId + " was not found");
			Command.addFieldValue(result, "item", item.toString(), "text-multi", "Item");
		} else {
			throw new PubSubException(Authorization.FORBIDDEN, "You do not have enough " +
				"permissions to retrieve item from a node.");
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
