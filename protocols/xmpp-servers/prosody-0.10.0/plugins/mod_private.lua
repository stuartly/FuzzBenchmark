-- Prosody IM
-- Copyright (C) 2008-2010 Matthew Wild
-- Copyright (C) 2008-2010 Waqas Hussain
--
-- This project is MIT/X11 licensed. Please see the
-- COPYING file in the source package for more information.
--


local st = require "util.stanza"

local private_storage = module:open_store();

module:add_feature("jabber:iq:private");

module:hook("iq/self/jabber:iq:private:query", function(event)
	local origin, stanza = event.origin, event.stanza;
	local query = stanza.tags[1];
	if #query.tags ~= 1 then
		origin.send(st.error_reply(stanza, "modify", "bad-format"));
		return true;
	end
	local tag = query.tags[1];
	local key = tag.name..":"..tag.attr.xmlns;
	local data, err = private_storage:get(origin.username);
	if err then
		origin.send(st.error_reply(stanza, "wait", "internal-server-error", err));
		return true;
	end
	if stanza.attr.type == "get" then
		if data and data[key] then
			origin.send(st.reply(stanza):query("jabber:iq:private"):add_child(st.deserialize(data[key])));
			return true;
		else
			origin.send(st.reply(stanza):add_child(query));
			return true;
		end
	else -- type == set
		if not data then data = {}; end;
		if #tag == 0 then
			data[key] = nil;
		else
			data[key] = st.preserialize(tag);
		end
		-- TODO delete datastore if empty
		local ok, err = private_storage:set(origin.username, data);
		if not ok then
			origin.send(st.error_reply(stanza, "wait", "internal-server-error", err));
			return true;
		end
		origin.send(st.reply(stanza));
		return true;
	end
end);
