-- * Metronome IM *
--
-- This file is part of the Metronome XMPP server and is released under the
-- ISC License, please see the LICENSE file in this source package for more
-- information about copyright and licensing.
--
-- As per the sublicensing clause, this file is also MIT/X11 Licensed.
-- ** Copyright (c) 2009-2012, Kim Alvefur, Florian Zeitz, Glenn Maynard, Jeff Mitchell, Matthew Wild, Waqas Hussain
--
-- Additional Contributors: Alban Bedel

local hosts = _G.hosts;
local st = require "util.stanza";
local datamanager = require "util.datamanager";
local dataform_new = require "util.dataforms".new;
local usermanager_user_exists = require "core.usermanager".user_exists;
local usermanager_create_user = require "core.usermanager".create_user;
local usermanager_set_password = require "core.usermanager".set_password;
local usermanager_delete_user = require "core.usermanager".delete_user;
local os_time = os.time;
local nodeprep = require "util.encodings".stringprep.nodeprep;
local jid_bare = require "util.jid".bare;
local new_ip = require "util.ip".new_ip;
local parse_subnets = require "util.ip".parse_subnets;
local match_subnet = require "util.ip".match_subnet;

local compat = module:get_option_boolean("registration_compat", true);
local allow_registration = module:get_option_boolean("allow_registration", false);
local additional_fields = module:get_option("additional_registration_fields", {});

local field_map = {
	username = { name = "username", type = "text-single", label = "Username", required = true };
	password = { name = "password", type = "text-private", label = "Password", required = true };
	nick = { name = "nick", type = "text-single", label = "Nickname" };
	name = { name = "name", type = "text-single", label = "Full Name" };
	first = { name = "first", type = "text-single", label = "Given Name" };
	last = { name = "last", type = "text-single", label = "Family Name" };
	email = { name = "email", type = "text-single", label = "Email" };
	address = { name = "address", type = "text-single", label = "Street" };
	city = { name = "city", type = "text-single", label = "City" };
	state = { name = "state", type = "text-single", label = "State" };
	zip = { name = "zip", type = "text-single", label = "Postal code" };
	phone = { name = "phone", type = "text-single", label = "Telephone number" };
	url = { name = "url", type = "text-single", label = "Webpage" };
	date = { name = "date", type = "text-single", label = "Birth date" };
};

local registration_form = dataform_new{
	title = "Creating a new account";
	instructions = "Choose a username and password for use with this service.";

	field_map.username;
	field_map.password;
};

local registration_query = st.stanza("query", {xmlns = "jabber:iq:register"})
	:tag("instructions"):text("Choose a username and password for use with this service."):up()
	:tag("username"):up()
	:tag("password"):up();

for _, field in ipairs(additional_fields) do
	if type(field) == "table" then
		registration_form[#registration_form + 1] = field;
	else
		if field:match("%+$") then
			field = field:sub(1, #field - 1);
			field_map[field].required = true;
		end

		registration_form[#registration_form + 1] = field_map[field];
		registration_query:tag(field):up();
	end
end
registration_query:add_child(registration_form:form());

module:add_feature("jabber:iq:register");

local register_stream_feature = st.stanza("register", {xmlns = "http://jabber.org/features/iq-register"}):up();
module:hook("stream-features", function(event)
	local session, features = event.origin, event.features;

	-- Advertise registration to unauthorized clients only.
	if not(allow_registration) or session.type ~= "c2s_unauthed" then
		return;
	end

	features:add_child(register_stream_feature);
end, 100);

local function handle_registration_stanza(event)
	local session, stanza = event.origin, event.stanza;

	local query = stanza.tags[1];
	if stanza.attr.type == "get" then
		local reply = st.reply(stanza);
		reply:tag("query", {xmlns = "jabber:iq:register"})
			:tag("registered"):up()
			:tag("username"):text(session.username):up()
			:tag("password"):up();
		session.send(reply);
	else -- stanza.attr.type == "set"
		if query.tags[1] and query.tags[1].name == "remove" then
			local username, host = session.username, session.host;

			local _close_session = session.close;
			session.close = function(session, ...)
				session.send(st.reply(stanza));
				return _close_session(session, ...);
			end
			
			local ok, err = usermanager_delete_user(username, host, "mod_register");
			
			if not ok then
				session.close = _close_session;
				module:log("debug", "Removing user account %s@%s failed: %s", username, host, err);
				session.send(st.error_reply(stanza, "cancel", "service-unavailable", err));
				return true;
			end

			module:log("info", "User removed their account: %s@%s", username, host);
		else
			local username = nodeprep(query:get_child("username"):get_text());
			local password = query:get_child("password"):get_text();
			if username and password then
				if username == session.username then
					if usermanager_set_password(username, password, session.host) then
						session.send(st.reply(stanza));
					else
						-- TODO unable to write file, file may be locked, etc, what's the correct error?
						session.send(st.error_reply(stanza, "wait", "internal-server-error"));
					end
				else
					session.send(st.error_reply(stanza, "modify", "bad-request"));
				end
			else
				session.send(st.error_reply(stanza, "modify", "bad-request"));
			end
		end
	end
	return true;
end

module:hook("iq/self/jabber:iq:register:query", handle_registration_stanza);
if compat then
	module:hook("iq/host/jabber:iq:register:query", function (event)
		local session, stanza = event.origin, event.stanza;
		if session.type == "c2s" and jid_bare(stanza.attr.to) == session.host then
			return handle_registration_stanza(event);
		end
	end);
end

local function parse_response(query)
	local form = query:get_child("x", "jabber:x:data");
	if form then
		return registration_form:data(form);
	else
		local data = {};
		local errors = {};
		for _, field in ipairs(registration_form) do
			local name, required = field.name, field.required;
			if field_map[name] then
				data[name] = query:get_child_text(name);
				if (not data[name] or #data[name] == 0) and required then
					errors[name] = "Required value missing";
				end
			end
		end
		if next(errors) then
			return data, errors;
		end
		return data;
	end
end

local recent_ips = {};
local min_seconds_between_registrations = module:get_option("min_seconds_between_registrations");
local match_type = module:get_option_string("registration_ip_match_type", "range");
if match_type ~= "range" and match_type ~= "single" then match_type = "range" end
local whitelist_only = module:get_option_boolean("whitelist_registration_only", false);
local whitelisted_ips = module:get_option_table("registration_whitelist", { "127.0.0.1" });
local blacklisted_ips = module:get_option_table("registration_blacklist", {});
if match_type ~= "single" then
	whitelisted_ips, blacklisted_ips = parse_subnets(whitelisted_ips), parse_subnets(blacklisted_ips);
end 

local function match_ip(ip, whitelist)
	local ips = whitelist and whitelisted_ips or blacklisted_ips;
	if match_type == "range" then
		ip = new_ip(ip);
		if ip then
			for _, s in ipairs(ips) do
				if match_subnet(ip, s.addr, s.mask) then
					return true;
				end
			end
		end
	else
		for _, address in ipairs(ips) do
			if address == ip then return true; end
		end
	end
	return false;
end

module:hook("stanza/iq/jabber:iq:register:query", function(event)
	local session, stanza = event.origin, event.stanza;

	if not allow_registration or session.type ~= "c2s_unauthed" then
		session.send(st.error_reply(stanza, "cancel", "service-unavailable"));
	else
		local query = stanza.tags[1];
		if stanza.attr.type == "get" then
			local reply = st.reply(stanza);
			reply:add_child(registration_query);
			session.send(reply);
		elseif stanza.attr.type == "set" then
			if query.tags[1] and query.tags[1].name == "remove" then
				session.send(st.error_reply(stanza, "auth", "registration-required"));
			else
				local data, errors = parse_response(query);
				if errors then
					session.send(st.error_reply(stanza, "modify", "not-acceptable"));
				else
					-- Check that the user is not blacklisted or registering too often
					if match_ip(session.ip) or (whitelist_only and not match_ip(session.ip, true)) then
						session.send(st.error_reply(stanza, "cancel", "not-acceptable", "You are not allowed to register an account."));
						return true;
					elseif min_seconds_between_registrations and not match_ip(session.ip, true) then
						if not recent_ips[session.ip] then
							recent_ips[session.ip] = { time = os_time(), count = 1 };
						else
							local ip = recent_ips[session.ip];
							ip.count = ip.count + 1;
							
							if os_time() - ip.time < min_seconds_between_registrations then
								ip.time = os_time();
								session.send(st.error_reply(stanza, "wait", "not-acceptable"));
								return true;
							end
							ip.time = os_time();
						end
					end
					local username, password = nodeprep(data.username), data.password;
					data.username, data.password = nil, nil;
					local host = module.host;
					if not username or username == "" then
						session.send(st.error_reply(stanza, "modify", "not-acceptable", "The requested username is invalid."));
					elseif usermanager_user_exists(username, host) then
						session.send(st.error_reply(stanza, "cancel", "conflict", "The requested username already exists."));
					else
						-- TODO unable to write file, file may be locked, etc, what's the correct error?
						local error_reply = st.error_reply(stanza, "wait", "internal-server-error", "Failed to write data to disk.");
						if usermanager_create_user(username, password, host) then
							if next(data) and not datamanager.store(username, host, "account_details", data) then
								usermanager_delete_user(username, host);
								session.send(error_reply);
								return true;
							end
							session.send(st.reply(stanza));
							module:log("info", "User account created: %s@%s", username, host);
							module:fire_event("user-registered", {
								username = username, host = host, source = "mod_register",
								session = session });
						else
							session.send(error_reply);
						end
					end
				end
			end
		end
	end
	return true;
end);
