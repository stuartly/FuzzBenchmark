-- * Metronome IM *
--
-- This file is part of the Metronome XMPP server and is released under the
-- ISC License, please see the LICENSE file in this source package for more
-- information about copyright and licensing.

local datamanager = datamanager
local b64_encode = require "util.encodings".base64.encode
local http_event = require "net.http.server".fire_server_event
local http_request = require "net.http".request
local jid_prep = require "util.jid".prep
local json_decode = require "util.json".decode
local nodeprep = require "util.encodings".stringprep.nodeprep
local ipairs, pairs, pcall, open, os_time, setmt, tonumber = 
      ipairs, pairs, pcall, io.open, os.time, setmetatable, tonumber
local sha1 = require "util.hashes".sha1
local urldecode = http.urldecode
local usermanager = usermanager
local generate = require "util.auxiliary".generate_secret
local timer = require "util.timer"

module:depends("http")

-- Pick up configuration and setup stores/variables.

local auth_token = module:get_option_string("reg_servlet_auth_token")
local secure = module:get_option_boolean("reg_servlet_secure", true)
local base_path = module:get_option_string("reg_servlet_base", "/register_account/")
local throttle_time = module:get_option_number("reg_servlet_ttime", nil)
local whitelist = module:get_option_set("reg_servlet_wl", {})
local blacklist = module:get_option_set("reg_servlet_bl", {})
local min_pass_len = module:get_option_number("reg_servlet_min_pass_length", 8)
local max_pass_len = module:get_option_number("reg_servlet_max_pass_length", 30)
local fm_patterns = module:get_option_table("reg_servlet_filtered_mails", {})
local fn_patterns = module:get_option_table("reg_servlet_filtered_nodes", {})
local use_nameapi = module:get_option_boolean("reg_servlet_use_nameapi", false)
local nameapi_ak = module:get_option_string("reg_servlet_nameapi_apikey")
local plain_errors = module:get_option_boolean("reg_servlet_plain_http_errors", false)
if use_nameapi and not nameapi_ak then use_nameapi = false end

local files_base = module.path:gsub("/[^/]+$","") .. "/template/"

local valid_files = {
	["css/style.css"] = files_base.."css/style.css",
	["images/tile.png"] = files_base.."images/tile.png",
	["images/header.png"] = files_base.."images/header.png"
}
local recent_ips = {}
local pending = {}
local pending_node = {}
local reset_tokens = {}
local default_whitelist, whitelisted, dea_checks;

if use_nameapi then
	default_whitelist = {
		["fastmail.fm"] = true,
		["gmail.com"] = true,
		["yahoo.com"] = true,
		["hotmail.com"] = true,
		["live.com"] = true,
		["icloud.com"] = true,
		["me.com"] = true
	}
	whitelisted = datamanager.load("register_json", module.host, "whitelisted_md") or default_whitelist
	dea_checks = {}
end

-- Setup hashes data structure

hashes = { _index = {} }
local hashes_mt = {} ; hashes_mt.__index = hashes_mt

function hashes_mt:add(node, mail)
	local _hash = b64_encode(sha1(mail))
	if not self[_hash] then
		-- check for eventual dupes
		self:remove(node, true);
		self[_hash] = node ; self._index[node] = _hash ; self:save()
		return true
	else
		return false
	end
end

function hashes_mt:remove(node, check)
	local _hash = self._index[node]
	if _hash then
		self[_hash] = nil ; self._index[node] = nil
	end
	if not check then self:save() end
end

function hashes_mt:save()
	if not datamanager.store("register_json", module.host, "hashes", hashes) then
		module:log("error", "Failed to save the mail addresses' hashes store")
	end
end

-- Utility functions

local function generate_secret(bytes)
	local str = generate(bytes)
	
	if not str or str:len() < 20 then
		repeat str = generate(bytes) until not str or str:len() >= 20
	end
	
	if not str then -- System issue just abort it
		return nil
	end

	return str
end

local function check_mail(address)
	for _, pattern in ipairs(fm_patterns) do 
		if address:match(pattern) then return false end
	end
	return true
end

local api_url = "http://rc50-api.nameapi.org/rest/v5.0/email/disposableemailaddressdetector?apiKey=%s&emailAddress=%s"
local function check_dea(address, username)
	local domain = address:match("@+(.*)$")
	if whitelisted[domain] then return end	

	module:log("debug", "Submitting domain to NameAPI for checking...")
	http_request(api_url:format(nameapi_ak, address), nil, function(data, code)
		if code == 200 then
			local ret = json_decode(data)
			if not ret then
				module:log("warn", "Failed to decode data from API, assuming address from %s as DEA...", domain)
				dea_checks[username] = true
				return
			end

			if ret.disposable == "YES" then
				dea_checks[username] = true
			else
				module:log("debug", "Mail domain %s is valid, whitelisting", domain)
				whitelisted[domain] = true
				datamanager.store("register_json", module.host, "whitelisted_md", whitelisted)
			end
		end	
	end)
end

local function check_node(node)
	for _, pattern in ipairs(fn_patterns) do
		if node:match(pattern) then return false end
	end
	return true
end

local function to_throttle(ip)
	if whitelist:contains(ip) then return true end
	if not recent_ips[ip] then
		recent_ips[ip] = os_time()
	else 
		if os_time() - recent_ips[ip] < throttle_time then
			recent_ips[ip] = os_time()
			return true;
		end
		recent_ips[ip] = os_time()
	end
	return false;
end

local function open_file(file)
	local f, err = open(file, "rb")
	if not f then return nil end

	local data = f:read("*a") ; f:close()
	return data
end

local function r_template(event, type)
	local data = open_file(files_base..type.."_t.html")
	if data then
		event.response.headers["Content-Type"] = "application/xhtml+xml"
		data = data:gsub("%%REG_URL%%", base_path..type:match("^(.*)_").."/")
		data = data:gsub("%%MIN_LEN%%", tostring(min_pass_len))
		data = data:gsub("%%MAX_LEN%%", tostring(max_pass_len))
		return data
	else return http_error_reply(event, 500, "Failed to obtain template.") end
end

local function http_file_get(event, type, path)
	if path == "" then return r_template(event, type.."_form") end		

	if valid_files[path] then
		local data = open_file(valid_files[path])
		if data then return data
		else return http_error_reply(event, 404, "Not found.") end
	end
end

local function http_error_reply(event, code, message, headers)
	local response = event.response

	if headers then
		for header, data in pairs(headers) do response.headers[header] = data end
	end

	response.status_code = code
	if plain_errors then
		response.headers["Content-Type"] = nil
		response:send(message)
	else
		response.headers["Content-Type"] = "text/html"
		response:send(http_event("http-error", { code = code, message = message }))
	end
end

-- Handlers

local function handle_register(data, event)
	-- Set up variables
	local username, password, ip, mail, token = data.username, data.password, data.ip, data.mail, data.auth_token

	-- Blacklist can be checked here.
	if blacklist:contains(ip) then 
		module:log("warn", "Attempt of reg. submission to the JSON servlet from blacklisted address: %s", ip)
		return http_error_reply(event, 403, "The specified address is blacklisted, sorry.") 
	end

	if not check_mail(mail) then
		module:log("warn", "%s attempted to use an invalid mail address (%s).", ip, mail)
		return http_error_reply(event, 403, "Requesting to register using this E-Mail address is forbidden, sorry.")
	end

	-- We first check if the supplied username for registration is already there.
	-- And nodeprep the username
	username = nodeprep(username)
	if not username then
		module:log("debug", "A username containing invalid characters was supplied: %s", data.username)
		return http_error_reply(event, 406, "Supplied username contains invalid characters, see RFC 6122.")
	else
		if not check_node(username) then
			module:log("warn", "%s attempted to use an username (%s) matching one of the forbidden patterns", ip, username)
			return http_error_reply(event, 403, "Requesting to register using this Username is forbidden, sorry.")
		end
			
		if pending_node[username] then
			module:log("warn", "%s attempted to submit a registration request but another request for that user (%s) is pending", ip, username)
			return http_error_reply(event, 401, "Another user registration by that username is pending.")
		end

		if password:len() < min_pass_len then
			module:log("debug", "%s submitted password is not long enough minimun is %d characters", ip, min_pass_len)
			return http_error_reply(event, 406, "Supplied password is not long enough minimum is " .. tostring(min_pass_len) .. " characters.")
		elseif password:len() > max_pass_len then
			module:log("debug", "%s submitted password is exceeding max length (%d characters)", ip, max_pass_len)
			return http_error_reply(event, 406, "Supplied password is exceeding max length (" .. tostring(max_pass_len) .. " characters).")
		end

		if not usermanager.user_exists(username, module.host) then
			-- if username fails to register successive requests shouldn't be throttled until one is successful.
			if throttle_time and to_throttle(ip) then
				module:log("warn", "JSON Registration request from %s has been throttled", ip)
				return http_error_reply(event, 503, "Request throttled, wait a bit and try again.")
			end
			
			if not hashes:add(username, mail) then
				module:log("warn", "%s (%s) attempted to register to the server with an E-Mail address we already possess the hash of", username, ip)
				return http_error_reply(event, 409, "The E-Mail Address provided matches the hash associated to an existing account.")
			end

			local id_token = generate_secret(20)
			if not id_token then
				module:log("error", "Failed to pipe from /dev/urandom to generate the account registration token")
				return http_error_reply(event, 500, "The xmpp server encountered an error trying to fullfil your request, please try again later.")
			end

			-- asynchronously run dea filtering if applicable
			if use_nameapi then check_dea(mail, username) end

			pending[id_token] = { node = username, password = password, ip = ip }
			pending_node[username] = id_token

			timer.add_task(300, function()
				if use_nameapi then dea_checks[username] = nil end
				if pending[id_token] then
					pending[id_token] = nil
					pending_node[username] = nil
					hashes:remove(username)
				end
			end)
			module:log("info", "%s (%s) submitted a registration request and is awaiting final verification", username, id_token)
			return id_token
		else
			module:log("debug", "%s registration data submission failed (user already exists)", username)
			return http_error_reply(event, 409, "User already exists.")
		end
	end
end

local function handle_password_reset(data, event)
	local mail, ip = data.reset, data.ip

	if throttle_time and to_throttle(ip) then
		module:log("warn", "JSON Password Reset request from %s has been throttled", ip)
		return http_error_reply(event, 503, "Request throttled, wait a bit and try again.")
	end
	
	local node = hashes[b64_encode(sha1(mail))]
	if node and usermanager.user_exists(node, module.host) then
		local id_token = generate_secret(20)
		if not id_token then
			module:log("error", "Failed to pipe from /dev/urandom to generate the password reset token")
			return http_error_reply(event, 500, "The xmpp server encountered an error trying to fullfil your request, please try again later.")
		end
		reset_tokens[id_token] = { node = node }
	
		timer.add_task(300, function()
			reset_tokens[id_token] = nil
		end)
		
		module:log("info", "%s submitted a password reset request, waiting for the change", node);
		return id_token
	else
		if node then hashes:remove(node) end -- user got deleted.
		module:log("warn", "%s submitted a password reset request for a mail address which has no account association (%s)", ip, mail);
		return http_error_reply(event, 404, "No account associated with the specified E-Mail address found.")
	end
end

local function handle_req(event)
	local request = event.request
	if secure and not request.secure then return nil end

	if request.method ~= "POST" then
		return http_error_reply(event, 405, "Bad method.", {["Allow"] = "POST"})
	end
	
	local data
	-- We check that what we have is valid JSON wise else we throw an error...
	if not pcall(function() data = json_decode(request.body) end) then
		module:log("debug", "Data submitted by %s failed to Decode", user)
		return http_error_reply(event, 400, "Decoding failed.")
	end
	
	-- Check if user is an admin of said host
	if data.auth_token ~= auth_token then
		module:log("warn", "%s tried to retrieve a registration token for %s@%s", request.ip, username, module.host)
		return http_error_reply(event, 401, "Auth token is invalid! The attempt has been logged.")
	else
		data.auth_token = nil;
	end
	
	-- Decode JSON data and check that all bits are there else throw an error
	if data.username and data.password and data.ip and data.mail then
		return handle_register(data, event);
	elseif data.reset and data.ip then
		return handle_password_reset(data, event);
	else
		module:log("debug", "A request with an insufficent number of elements was sent")
		return http_error_reply(event, 400, "Invalid syntax.")
	end
end

local function handle_reset(event, path)
	local request = event.request
	local body = request.body
	if secure and not request.secure then return nil end
	
	if request.method == "GET" then
		return http_file_get(event, "reset", path)
	elseif request.method == "POST" then
		if path == "" then
			if not body then return http_error_reply(event, 400, "Bad Request.") end
			local id_token, password, verify = body:match("^id_token=(.*)&password=(.*)&verify=(.*)$")
			if id_token and password and verify then
				id_token, password, verify = urldecode(id_token), urldecode(password), urldecode(verify)
				if password ~= verify then 
					return r_template(event, "reset_nomatch")
				else
					local node = reset_tokens[id_token] and reset_tokens[id_token].node
					if node then
						if not (password:find("%d+") and password:find("%u+")) or 
							password:len() < min_pass_len or password:len() > max_pass_len then
							return r_template(event, "reset_password_check")
						end

						local ok, error = usermanager.set_password(node, password, module.host)
						if ok then
							module:log("info", "User %s successfully changed the account password", node)
							module:fire_event(
								"user-changed-password", 
								{ username = node, host = module.host, id_token = id_token, password = password, source = "mod_register_json" }
							)
							reset_tokens[id_token] = nil
							return r_template(event, "reset_success")
						else
							module:log("error", "Password change for %s failed: %s", node, error)
							return http_error_reply(event, 500, "Encountered an error while changing the password: "..error)
						end
					else
						return r_template(event, "reset_fail")
					end
				end
			else
				return http_error_reply(event, 400, "Invalid Request.")
			end
		end
	else
		return http_error_reply(event, 405, "Invalid method.")
	end
end

local function handle_verify(event, path)
	local request = event.request
	local body = request.body
	if secure and not request.secure then return nil end

	if request.method == "GET" then
		return http_file_get(event, "verify", path)
	elseif request.method == "POST" then
		if path == "" then
			if not body then return http_error_reply(event, 400, "Bad Request.") end
			local id_token = urldecode(body):match("^id_token=(.*)$")

			if not pending[id_token] then
				return r_template(event, "verify_fail")
			else
				local username, password, ip = 
				      pending[id_token].node, pending[id_token].password, pending[id_token].ip

				if use_nameapi and dea_checks[username] then
					module:log("warn", "%s (%s) attempted to register using a disposable mail address, denying", username, ip)
					pending[id_token] = nil ; pending_node[username] = nil ; dea_checks[username] = nil ; hashes:remove(username)
					return r_template(event, "verify_fail")
				end

				local ok, error = usermanager.create_user(username, password, module.host)
				if ok then 
					module:fire_event(
						"user-registered", 
						{ username = username, host = module.host, id_token = id_token, password = password, source = "mod_register_json", session = { ip = ip } }
					)
					module:log("info", "Account %s@%s is successfully verified and activated", username, module.host)
					-- we shall not clean the user from the pending lists as long as registration doesn't succeed.
					pending[id_token] = nil ; pending_node[username] = nil
					return r_template(event, "verify_success")				
				else
					module:log("error", "User creation failed: "..error)
					return http_error_reply(event, 500, "Encountered an error while creating the user: "..error)
				end
			end
		end	
	else
		return http_error_reply(event, 405, "Invalid method.")
	end
end

local function handle_user_deletion(event)
	local user, hostname = event.username, event.host
	if hostname == module.host then hashes:remove(user) end
end

local function slash_redirect(event)
	event.response.headers.location = event.request.path .. "/";
	return 301;
end

-- Set it up!

hashes = datamanager.load("register_json", module.host, "hashes") or hashes ; setmt(hashes, hashes_mt)

module:provides("http", {
	default_path = base_path,
	route = {
		["GET /"] = handle_req,
		["POST /"] = handle_req,
		["GET /reset"] = slash_redirect,
		["GET /verify"] = slash_redirect,
		["GET /reset/*"] = handle_reset,
		["POST /reset/*"] = handle_reset,
		["GET /verify/*"] = handle_verify,
		["POST /verify/*"] = handle_verify
	}
})

module:hook_global("user-deleted", handle_user_deletion, 10);

-- Reloadability

module.save = function() return { hashes = hashes, whitelisted = whitelisted } end
module.restore = function(data) 
	hashes = data.hashes or { _index = {} } ; setmt(hashes, hashes_mt)
	whitelisted = use_nameapi and (data.whitelisted or default_whitelist) or nil
end
