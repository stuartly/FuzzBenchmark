-- * Metronome IM *
--
-- This file is part of the Metronome XMPP server and is released under the
-- ISC License, please see the LICENSE file in this source package for more
-- information about copyright and licensing.
--
-- As per the sublicensing clause, this file is also MIT/X11 Licensed.
-- ** Copyright (c) 2011-2013, Kim Alvefur, Marco Cirillo, Matthew Wild, Waqas Hussain

module:set_global();
module:depends("http_errors");

local moduleapi = require "core.moduleapi";
local url_parse = require "socket.url".parse;
local url_build = require "socket.url".build;

local server = require "net.http.server";

server.set_default_host(module:get_option_string("http_default_host"));
server.set_alias_hosts(module:get_option_table("http_alias_hosts", {}));

local function normalize_path(path)
	if path:sub(-1,-1) == "/" then path = path:sub(1, -2); end
	if path:sub(1,1) ~= "/" then path = "/"..path; end
	return path;
end

local function get_http_event(host, app_path, key)
	local method, path = key:match("^(%S+)%s+(.+)$");
	if not method then -- No path specified, default to "" (base path)
		method, path = key, "";
	end
	if method:sub(1,1) == "/" then
		return nil;
	end
	if app_path == "/" and path:sub(1,1) == "/" then
		app_path = "";
	end
	return method:upper().." "..host..app_path..path;
end

local function get_base_path(host_module, app_name, default_app_path)
	return normalize_path(host_module:get_option("http_paths", {})[app_name] -- Host
		or module:get_option("http_paths", {})[app_name] -- Global
		or default_app_path); -- Default
end

local ports_by_scheme = { http = 80, https = 443, };

-- Helper to deduce a module's external URL
function moduleapi.http_url(module, app_name, default_path)
	app_name = app_name or (module.name:gsub("^http_", ""));
	local ext = url_parse(module:get_option_string("http_external_url")) or {};
	local services = portmanager.get_active_services();
	local http_services = services:get("https") or services:get("http") or {};
	for interface, ports in pairs(http_services) do
		for port, services in pairs(ports) do
			local url = {
				scheme = (ext.scheme or services[1].service.name);
				host = (ext.host or module.host);
				port = tonumber(ext.port) or port or 80;
				path = normalize_path(ext.path or "/")..
					((default_path and default_path) or
					(get_base_path(module, app_name, "/"..app_name)):sub(2));
			}
			if ports_by_scheme[url.scheme] == url.port then url.port = nil end
			return url_build(url);
		end
	end
end

function module.add_host(module)
	module:depends("favicon");

	local host = module.host;
	local apps = {};
	module.environment.apps = apps;
	local function http_app_added(event)
		local app_name = event.item.name;
		local default_app_path = event.item.default_path or "/"..app_name;
		local app_path = get_base_path(module, app_name, default_app_path);
		if not app_name then
			-- TODO: Link to docs
			module:log("error", "HTTP app has no 'name', add one or use module:provides('http', app)");
			return;
		end
		apps[app_name] = apps[app_name] or {};
		local app_handlers = apps[app_name];
		for key, handler in pairs(event.item.route or {}) do
			local event_name = get_http_event(host, app_path, key);
			if event_name then
				if type(handler) ~= "function" then
					local data = handler;
					handler = function () return data; end
				elseif event_name:sub(-2, -1) == "/*" then
					local base_path = event_name:match("/(.+)/*$");
					local _handler = handler;
					handler = function (event)
						local path = event.request.path:sub(#base_path+1);
						return _handler(event, path);
					end;
				end
				if not app_handlers[event_name] then
					app_handlers[event_name] = handler;
					module:hook_object_event(server, event_name, handler);
				else
					module:log("warn", "App %s added handler twice for '%s', ignoring", app_name, event_name);
				end
			else
				module:log("error", "Invalid route in %s, %q.", app_name, key);
			end
		end
	end
	
	local function http_app_removed(event)
		local app_handlers = apps[event.item.name];
		apps[event.item.name] = nil;
		for event, handler in pairs(app_handlers) do
			module:unhook_object_event(server, event, handler);
		end
	end
	
	module:handle_items("http-provider", http_app_added, http_app_removed);

	server.add_host(host);
	function module.unload()
		server.remove_host(host);
	end
end

module:add_item("net-provider", {
	name = "http";
	listener = server.listener;
	default_port = 5280;
	multiplex = {
		pattern = "^[A-Z]";
	};
});

module:add_item("net-provider", {
	name = "https";
	listener = server.listener;
	encryption = "ssl";
	ssl_config = { verify = "none" };
	multiplex = {
		pattern = "^[A-Z]";
	};
});
