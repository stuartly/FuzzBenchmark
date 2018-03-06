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

\i database/postgresql-pubsub-schema-3.1.0.sql

-- LOAD FILE: database/postgresql-pubsub-schema-3.1.0.sql

-- QUERY START:
drop function if exists TigPubSubCreateNode(varchar,varchar,int,varchar,text,bigint);
-- QUERY END:

-- QUERY START:
create or replace function TigPubSubCreateNode(_service_jid varchar(2049), _node_name varchar(1024), _node_type int, _node_creator varchar(2049), _node_conf text, _collection_id bigint) returns bigint as $$
declare
    _service_id bigint;
    _node_creator_id bigint;
    _node_id bigint;
begin
	select TigPubSubEnsureServiceJid(_service_jid) into _service_id;
	select TigPubSubEnsureJid(_node_creator) into _node_creator_id;
	insert into tig_pubsub_nodes (service_id, name, "type", creator_id, creation_date, configuration, collection_id)
		values (_service_id, _node_name, _node_type, _node_creator_id, now(), _node_conf, _collection_id);
	select currval('tig_pubsub_nodes_node_id_seq') into _node_id;
	return _node_id;
end;
$$ LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigPubSubGetNodeMeta(_service_jid varchar(2049), _node_name varchar(1024)) returns table (
    node_id bigint,
    configuration text,
    creator varchar(2049),
    creation_date timestamp
) as $$
begin
    return query select n.node_id, n.configuration, cj.jid, n.creation_date
        from tig_pubsub_nodes n
		    inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
		    inner join tig_pubsub_jids cj on cj.jid_id = n.creator_id
		    where sj.service_jid = _service_jid and n.name = _node_name;
end ;
$$ LANGUAGE 'plpgsql';
-- QUERY END:
