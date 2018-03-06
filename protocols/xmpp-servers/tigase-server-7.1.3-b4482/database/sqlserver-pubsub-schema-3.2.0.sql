--
--  Tigase PubSub Component
--  Copyright (C) 2016 "Tigase, Inc." <office@tigase.com>
--
--  This program is free software: you can redistribute it and/or modify
--  it under the terms of the GNU Affero General Public License as published by
--  the Free Software Foundation, either version 3 of the License.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU Affero General Public License for more details.
--
--  You should have received a copy of the GNU Affero General Public License
--  along with this program. Look for COPYING file in the top folder.
--  If not, see http://www.gnu.org/licenses/.

-- LOAD FILE: database/sqlserver-pubsub-schema-3.1.0.sql

-- QUERY START:
SET QUOTED_IDENTIFIER ON
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeMeta')
	DROP PROCEDURE TigPubSubGetNodeMeta
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeMeta
	@_service_jid nvarchar(2049),
	@_node_name nvarchar(1024)
AS
begin
    select n.node_id, n.configuration, cj.jid, n.creation_date
        from tig_pubsub_nodes n
		    inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
		    inner join tig_pubsub_jids cj on cj.jid_id = n.creator_id
		    where sj.service_jid_sha1 = HASHBYTES('SHA1', @_service_jid) and n.name_sha1 = HASHBYTES('SHA1', @_node_name)
		        and sj.service_jid = @_service_jid and n.name = @_node_name;
end
-- QUERY END:
GO
