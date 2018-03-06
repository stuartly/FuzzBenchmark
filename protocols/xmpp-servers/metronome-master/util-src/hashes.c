/*
-- * Metronome IM *
--
-- This file is part of the Metronome XMPP server and is released under the
-- ISC License, please see the LICENSE file in this source package for more
-- information about copyright and licensing.
--
-- As per the sublicensing clause, this file is also MIT/X11 Licensed.
-- ** Copyright (c) 2008-2012, Kim Alvefur, Matthew Wild, Tobias Markmann
*/

/*
* hashes.c
* Lua library for sha1, sha256 and md5 hashes
*/

#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include <openssl/sha.h>
#include <openssl/md5.h>

const char* hex_tab = "0123456789abcdef";
void toHex(const char* in, int length, char* out) {
	int i;
	for (i = 0; i < length; i++) {
		out[i*2] = hex_tab[(in[i] >> 4) & 0xF];
		out[i*2+1] = hex_tab[(in[i]) & 0xF];
	}
}

#define MAKE_HASH_FUNCTION(myFunc, func, size) \
static int myFunc(lua_State *L) { \
	size_t len; \
	const char *s = luaL_checklstring(L, 1, &len); \
	int hex_out = lua_toboolean(L, 2); \
	char hash[size]; \
	char result[size*2]; \
	func((const unsigned char*)s, len, (unsigned char*)hash);  \
	if (hex_out) { \
		toHex(hash, size, result); \
		lua_pushlstring(L, result, size*2); \
	} else { \
		lua_pushlstring(L, hash, size);\
	} \
	return 1; \
}

MAKE_HASH_FUNCTION(Lsha1, SHA1, SHA_DIGEST_LENGTH)
MAKE_HASH_FUNCTION(Lsha224, SHA224, SHA224_DIGEST_LENGTH)
MAKE_HASH_FUNCTION(Lsha256, SHA256, SHA256_DIGEST_LENGTH)
MAKE_HASH_FUNCTION(Lsha384, SHA384, SHA384_DIGEST_LENGTH)
MAKE_HASH_FUNCTION(Lsha512, SHA512, SHA512_DIGEST_LENGTH)
MAKE_HASH_FUNCTION(Lmd5, MD5, MD5_DIGEST_LENGTH)

static const luaL_Reg Reg[] =
{
	{ "sha1",	Lsha1	},
	{ "sha224",	Lsha224	},
	{ "sha256",	Lsha256	},
	{ "sha384",	Lsha384	},
	{ "sha512",	Lsha512	},
	{ "md5",	Lmd5	},
	{ NULL,		NULL	}
};

LUALIB_API int luaopen_util_hashes(lua_State *L)
{
	luaL_register(L, "hashes", Reg);
	lua_pushliteral(L, "version");			/** version */
	lua_pushliteral(L, "-3.14");
	lua_settable(L,-3);
	return 1;
}
