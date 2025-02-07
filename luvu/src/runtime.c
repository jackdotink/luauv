#include "runtime.h"

#include <stdio.h>

#include "lua.h"
#include "luacode.h"
#include "lualib.h"
#include "require.h"
#include "lib/task.h"
#include "uv.h"

lua_State* luvu_newruntime(uv_loop_t* loop) {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	static const luaL_Reg globals[] = {
		{"require", luvu_require},
		{NULL, NULL},
	};

	luaL_register(L, "_G", globals);
	lua_pop(L, 1);

	luvu_opentask(L);
	lua_pop(L, 1);

	luaL_sandbox(L);
	lua_setthreaddata(L, loop);

	return L;
}

lua_State* luvu_newthread(lua_State* L) {
	lua_State* thread = lua_newthread(L);
	luaL_sandboxthread(thread);

	return thread;
}

uv_loop_t* luvu_getloop(lua_State* L) {
	return lua_getthreaddata(lua_mainthread(L));
}

static void handlestatus(lua_State* L, int status) {
	if (status != LUA_OK && status != LUA_YIELD) {
		const char* error = lua_tostring(L, -1);
		if (error == NULL) {
			error = "unknown error";
		}

		fprintf(stderr, "%s\ntrace:\n%s", error, lua_debugtrace(L));
	}
}

void luvu_spawn(lua_State* L, lua_State* from, int nargs) {
	int status = lua_resume(L, from, nargs);
	handlestatus(L, status);
}

void luvu_spawnerror(lua_State* L, lua_State* from, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	lua_pushvfstring(L, fmt, args);
	va_end(args);

	int status = lua_resumeerror(L, from);
	handlestatus(L, status);
}

lua_State* luvu_execute(
	lua_State* main,
	const char* name,
	const char* code,
	size_t codesize
) {
	size_t bytecodesize;
	char* bytecode = luau_compile(code, codesize, NULL, &bytecodesize);

	lua_State* L = luvu_newthread(main);

	if (luau_load(L, name, bytecode, bytecodesize, 0) != 0) {
		const char* error = lua_tostring(L, -1);
		if (error == NULL) {
			error = "unknown error";
		}

		fprintf(stderr, "%s\n", error);
	}

	lua_pop(main, 1);
	luvu_spawn(L, NULL, 0);

	return L;
}
