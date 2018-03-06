-- * Metronome IM *
--
-- This file is part of the Metronome XMPP server and is released under the
-- ISC License, please see the LICENSE file in this source package for more
-- information about copyright and licensing.
--
-- As per the sublicensing clause, this file is also MIT/X11 Licensed.
-- ** Copyright (c) 2008-2013, Florian Zeitz, Kim Alvefur, Marco Cirillo, Matthew Wild

local config = require "core.configmanager";
local modulemanager = require "modulemanager";
local array = require "util.array";
local set = require "util.set";
local logger = require "util.logger";
local pluginloader = require "util.pluginloader";
local timer = require "util.timer";

local t_insert, t_remove, t_concat = table.insert, table.remove, table.concat;
local error, setmetatable, type = error, setmetatable, type;
local ipairs, pairs, select, unpack = ipairs, pairs, select, unpack;
local tonumber, tostring = tonumber, tostring;

local metronome = metronome;
local hosts = metronome.hosts;
local fire_global_event = metronome.events.fire_event;

local shared_data = setmetatable({}, { __mode = "v" });

local NULL = {};

local api = {};

function api:get_name()
	return self.name;
end

function api:get_host()
	return self.host;
end

function api:get_host_type()
	return self.host ~= "*" and hosts[self.host].type or nil;
end

function api:set_global()
	self.host = "*";
	local _log = logger.init("mod_"..self.name);
	self.log = function (self, ...) return _log(...); end;
	self._log = _log;
	self.global = true;
end

function api:set_component_inheritable()
	self.component_inheritable = true;
end

function api:add_feature(xmlns)
	self:add_item("feature", xmlns);
end
function api:add_identity(category, type, name)
	self:add_item("identity", {category = category, type = type, name = name});
end
function api:add_extension(data)
	self:add_item("extension", data);
end
function api:has_feature(xmlns, host)
	for _, feature in ipairs(self:get_items("feature", host)) do
		if feature == xmlns then return true; end
	end
	return false;
end
function api:has_identity(category, type, name, host)
	for _, id in ipairs(self:get_items("identity", host)) do
		if id.category == category and id.type == type and id.name == name then
			return true; 
		end
	end
	return false;
end

function api:fire_event(...)
	return (hosts[self.host] or metronome).events.fire_event(...);
end

function api:fire_global_event(...)
	return metronome.events.fire_event(...);
end

function api:hook_object_event(object, event, handler, priority)
	self.event_handlers:set(object, event, handler, true);
	return object.add_handler(event, handler, priority);
end

function api:unhook_object_event(object, event, handler)
	return object.remove_handler(event, handler);
end

function api:hook(event, handler, priority)
	return self:hook_object_event((hosts[self.host] or metronome).events, event, handler, priority);
end

function api:hook_global(event, handler, priority)
	return self:hook_object_event(metronome.events, event, handler, priority);
end

function api:hook_tag(xmlns, name, handler, priority)
	if not handler and type(name) == "function" then
		xmlns, name, handler, priority = nil, xmlns, name, handler;
	elseif not (handler and name) then
		self:log("warn", "Error: Insufficient parameters to module:hook_stanza()");
		return;
	end
	return self:hook("stanza/"..(xmlns and (xmlns..":") or "")..name, function (data) return handler(data.origin, data.stanza, data); end, priority);
end
api.hook_stanza = api.hook_tag;

function api:require(lib, path)
	local e;
	local f, n = pluginloader.load_code(path or self.name, lib..".lib.lua", self.environment);
	if not f then
		e = n;
		f, n = pluginloader.load_code(lib, lib..".lib.lua", self.environment);
	end
	if not f then error("Failed to load plugin library '"..lib.."', error 1: "..e..", error 2: "..n); end
	return f();
end

function api:depends(name)
	if not self.dependencies then
		self.dependencies = {};
		self:hook("module-reloaded", function (event)
			if self.dependencies[event.module] and not self.reloading then
				self:log("info", "Auto-reloading due to reload of %s:%s", event.host, event.module);
				modulemanager.reload(self.host, self.name);
				return;
			end
		end);
		self:hook("module-unloaded", function (event)
			if self.dependencies[event.module] then
				self:log("info", "Auto-unloading due to unload of %s:%s", event.host, event.module);
				modulemanager.unload(self.host, self.name);
			end
		end);
	end
	local mod = modulemanager.get_module(self.host, name) or modulemanager.get_module("*", name);
	if mod and mod.module.host == "*" and self.host ~= "*"
	and modulemanager.module_has_method(mod, "add_host") then
		mod = nil; -- Target is a shared module, so we still want to load it on our host
	end
	if not mod then
		local err;
		mod, err = modulemanager.load(self.host, name);
		if not mod then
			return error(("Unable to load required module, mod_%s: %s"):format(name, ((err or "unknown error"):gsub("%-", " ")) ));
		end
	end
	self.dependencies[name] = true;
	return mod;
end

function api:shared(...)
	if not self.shared_data then self.shared_data = {}; end
	local paths = { n = select("#", ...), ... };
	local data_array = {};
	local default_path_components = { self.host, self.name };
	for i = 1, paths.n do
		local path = paths[i];
		if path:sub(1,1) ~= "/" then
			local n_components = select(2, path:gsub("/", "%1"));
			path = (n_components<#default_path_components and "/" or "")..t_concat(default_path_components, "/", 1, #default_path_components-n_components).."/"..path;
		end
		local shared = shared_data[path];
		if not shared then
			shared = {};
			if path:match("%-cache$") then
				setmetatable(shared, { __mode = "kv" });
			end
			shared_data[path] = shared;
		end
		t_insert(data_array, shared);
		self.shared_data[path] = shared;
	end
	return unpack(data_array);
end

function api:get_option(name, default_value)
	local value = config.get(self.host, name);
	if value == nil then
		value = config.get("*", name);
		value = value == nil and default_value or value;
	end
	return value;
end

function api:get_option_string(name, default_value)
	local value = self:get_option(name, default_value);
	if type(value) == "table" then
		if #value > 1 then
			self:log("error", "Config option '%s' does not take a list, using just the first item", name);
		end
		value = value[1];
	end
	if value == nil then
		return nil;
	end
	return tostring(value);
end

function api:get_option_number(name, ...)
	local value = self:get_option(name, ...);
	if type(value) == "table" then
		if #value > 1 then
			self:log("error", "Config option '%s' does not take a list, using just the first item", name);
		end
		value = value[1];
	end
	local ret = tonumber(value);
	if value ~= nil and ret == nil then
		self:log("error", "Config option '%s' not understood, expecting a number", name);
	end
	return ret;
end

function api:get_option_boolean(name, ...)
	local value = self:get_option(name, ...);
	if type(value) == "table" then
		if #value > 1 then
			self:log("error", "Config option '%s' does not take a list, using just the first item", name);
		end
		value = value[1];
	end
	if value == nil then
		return nil;
	end
	local ret = value == true or value == "true" or value == 1 or nil;
	if ret == nil then
		ret = (value == false or value == "false" or value == 0);
		if ret then
			ret = false;
		else
			ret = nil;
		end
	end
	if ret == nil then
		self:log("error", "Config option '%s' not understood, expecting true/false", name);
	end
	return ret;
end

function api:get_option_array(name, ...)
	local value = self:get_option(name, ...);

	if value == nil then
		return nil;
	end
	
	if type(value) ~= "table" then
		return array{ value };
	end
	
	return array():append(value);
end

function api:get_option_set(name, ...)
	local value = self:get_option_array(name, ...);
	
	if value == nil then
		return nil;
	end
	
	return set.new(value);
end

function api:get_option_table(name, ...)
	local value = self:get_option(name, ...);

	if value == nil then
		return nil;
	end

	if type(value) ~= "table" then
		self:log("error", "Config option '%s' not understood, expecting a table", name);
		return nil;
	else
		return value;
	end
end

function api:add_item(key, value)
	self.items = self.items or {};
	self.items[key] = self.items[key] or {};
	t_insert(self.items[key], value);
	self:fire_event("item-added/"..key, {source = self, item = value});
end
function api:remove_item(key, value)
	local t = self.items and self.items[key] or NULL;
	for i = #t,1,-1 do
		if t[i] == value then
			t_remove(self.items[key], i);
			self:fire_event("item-removed/"..key, {source = self, item = value});
			return value;
		end
	end
end

function api:get_items(key, host)
	local result = modulemanager.get_items(key, host or self.host) or {};
	return result;
end

function api:handle_items(type, added_cb, removed_cb, existing)
	self:hook("item-added/"..type, added_cb);
	self:hook("item-removed/"..type, removed_cb);
	if existing ~= false then
		for _, item in ipairs(self:get_items(type)) do
			added_cb({ item = item });
		end
	end
end

function api:provides(name, item)
	if not item then item = self.environment; end
	if not item.name then
		local item_name = self.name;
		-- Strip a provider prefix to find the item name
		-- (e.g. "auth_foo" -> "foo" for an auth provider)
		if item_name:find(name.."_", 1, true) == 1 then
			item_name = item_name:sub(#name+2);
		end
		item.name = item_name;
	end
	self:add_item(name.."-provider", item);
end

function api:send(stanza)
	return fire_global_event("route/post", hosts[self.host], stanza);
end

function api:add_timer(delay, callback)
	return timer.add_task(delay, function (t)
		if self.loaded == false then return; end
		return callback(t);
	end);
end

local path_sep = package.config:sub(1,1);
function api:get_directory()
	return self.path and (self.path:gsub("%"..path_sep.."[^"..path_sep.."]*$", "")) or nil;
end

function api:load_resource(path, mode)
	path = config.resolve_relative_path(self:get_directory(), path);
	return io.open(path, mode);
end

return api;
