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
 */
/*
 Retrieve code for manual licence retrieval
 AS:Description: Retrieve code for licence
 AS:CommandId: retrieve-code-for-licence
 AS:Component: vhost-man
 AS:Group: Licence
 */
package tigase.admin

import tigase.server.Command
import tigase.server.Iq
import tigase.xml.Element

def p = (Iq)packet

def admins = (Set)adminsSet
def stanzaFromBare = p.getStanzaFrom().getBareJID()
def isServiceAdmin = admins.contains(stanzaFromBare)

if (!isServiceAdmin) {
	def result = p.commandResult(Command.DataType.result);
	Command.addTextField(result, "Error", "You are not service administrator");
	return result
}

String componentId = Command.getFieldValue(p, "component-id");
if (componentId == null) {
	def possibleValues = (tigase.licence.LicenceChecker.getLicencedComponents() as List).sort();

	def result = p.commandResult(Command.DataType.form);

	Command.addTitle(result, "Please select component ID for which you want to retrieve code required for manual licence retrieval")
	Command.addFieldValue(result, "component-id", "", "Component ID", possibleValues as String[], possibleValues as String[], "list-single")
	return result
}

def result = p.commandResult(Command.DataType.result)

Command.addTitle(result, "Licence code");
Command.addInstructions(result, "Licence code contains following information:")
tigase.licence.LicenceChecker.getLicencingDetails(componentId).getChildren().each { Element child ->
	Command.addFieldValue(result, child.getName(), child.getCData(), "text-single");
}

String code = tigase.licence.LicenceChecker.getCodeForLicenceRetrieval(componentId);
Command.addFieldValue(result, "Code", code, "text-multi");

return result;