#include <lua.h>
#include <lauxlib.h>

static int Lcreate_table(lua_State *L) {
	lua_createtable(L, luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));
	return 1;
}

static int Lpack(lua_State *L) {
	unsigned int n_args = lua_gettop(L);
	lua_createtable(L, n_args, 1);
	lua_insert(L, 1);

	for(int arg = n_args; arg >= 1; arg--) {
		lua_rawseti(L, 1, arg);
	}

	lua_pushinteger(L, n_args);
	lua_setfield(L, -2, "n");
	return 1;
}

int luaopen_util_table(lua_State *L) {
#if (LUA_VERSION_NUM > 501)
	luaL_checkversion(L);
#endif
	lua_createtable(L, 0, 2);
	lua_pushcfunction(L, Lcreate_table);
	lua_setfield(L, -2, "create");
	lua_pushcfunction(L, Lpack);
	lua_setfield(L, -2, "pack");
	return 1;
}
