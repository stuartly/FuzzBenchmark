/*
 * Tigase Jabber/XMPP Server
 * Copyright (C) 2004-2017 "Tigase, Inc." <office@tigase.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. Look for COPYING file in the top folder.
 * If not, see http://www.gnu.org/licenses/.
 */
/*
 Retrieve code for manual licence retrieval
 AS:Description: Force retrieving licence from the server
 AS:CommandId: retrieve-licence-from-server
 AS:Component: vhost-man
 AS:Group: Licence
 */
package tigase.admin

import tigase.licence.LicenceChecker
import tigase.server.Command
import tigase.server.Iq
import tigase.xml.Element

import java.security.NoSuchAlgorithmException
import java.security.spec.InvalidKeySpecException
import java.text.DateFormat
import java.text.SimpleDateFormat
import java.time.LocalDate

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
SimpleDateFormat sdf = new SimpleDateFormat( "yyyy-MM-dd" );
sdf.setTimeZone( TimeZone.getTimeZone("UTC") );

def checker = LicenceChecker.getLicenceChecker(componentId);
def validUntilOld = checker.getValidUntil();
def digestOld = checker.getLicenceDigest();

try {
	checker.reloadLicenceFromServer();

	def validUntilNew = checker.getValidUntil();
	def digestNew = checker.getLicenceDigest();

	Command.addTitle(result, "Licence code reload has been completed");

	Command.addFieldValue(result, "Previous licence valid-until", sdf.format(validUntilOld), "text-single");
	Command.addFieldValue(result, "Previous licence digest", digestOld, "text-single");
	Command.addFieldValue(result, "Loaded licence valid-until", sdf.format(validUntilNew), "text-single");
	Command.addFieldValue(result, "Loaded licence digest", digestNew, "text-single");

} catch ( Exception e) {
	Command.addTitle(result, "Reloading of the licence failed!");
}


return result;