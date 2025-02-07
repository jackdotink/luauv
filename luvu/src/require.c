#include "require.h"

#include <string.h>

#include "lua.h"
#include "lualib.h"

static const char REG_REQUIRE_KEY = 'r';

static void push_require_cache(lua_State* L) {
	lua_pushlightuserdata(L, (void*) &REG_REQUIRE_KEY);
	lua_rawget(L, LUA_REGISTRYINDEX);

	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);

		lua_newtable(L);

		lua_pushlightuserdata(L, (void*) &REG_REQUIRE_KEY);
		lua_pushvalue(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}
}

static void require_cache_get(lua_State* L, const char* path) {
	push_require_cache(L);
	lua_pushstring(L, path);
	lua_rawget(L, -2);
	lua_remove(L, -2);
}

static void require_cache_set(lua_State* L, const char* path) {
	push_require_cache(L);

	lua_pushstring(L, path);
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

static void load_std(lua_State* L, const char* path) {
	require_cache_get(L, path);
	if (!lua_isnil(L, -1)) {
		return;
	}

	if (strcmp(path, "@std/fs") == 0) {
		// todo
	} else {
		luaL_error(L, "no such module '%s'", path);
	}

	require_cache_set(L, path);
}

int luvu_require(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);

	if (path[0] != '@')
		luaL_error(L, "invalid require path '%s'; luauv only supports alias requires", path);

	if (strncmp(path, "@std", 4) == 0) {
		load_std(L, path);
		return 1;
	}

	luaL_error(L, "require is not yet implemented for user modules");

	return 1;
}
