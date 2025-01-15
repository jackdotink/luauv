#include "execute.h"

#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "luacode.h"
#include "lualib.h"
#include "require.h"
#include "scheduler.h"
#include "tasklib.h"
#include "uv.h"

lua_State* luauv_execute_newmainthread(uv_loop_t* loop) {
	lua_State* mainthread = luaL_newstate();
	luaL_openlibs(mainthread);

	static const luaL_Reg globals[] = {
		{"require", luauv_require},
		{NULL, NULL},
	};

	luaL_register(mainthread, "_G", globals);
	lua_pop(mainthread, 1);

	luauv_task_open(mainthread);
	lua_pop(mainthread, 1);

	luaL_sandbox(mainthread);
	lua_setthreaddata(mainthread, loop);

	return mainthread;
}

lua_State* luauv_execute_newthread(lua_State* L) {
	lua_State* thread = lua_newthread(L);
	luaL_sandboxthread(thread);

	return thread;
}

lua_State* luauv_execute_runfile(lua_State* mainthread, const char* filename, const char* code, size_t codesize) {
	size_t bytecodesize;
	char* bytecode = luau_compile(code, codesize, NULL, &bytecodesize);

	lua_State* L = luauv_execute_newthread(mainthread);

	if (luau_load(L, filename, bytecode, bytecodesize, 0) != 0) {
		const char* luaerror = lua_tostring(L, -1);
		if (luaerror == NULL) {
			luaerror = "unknown error";
		}

		printf("%s\n", luaerror);
	}

	luauv_scheduler_spawn(L, NULL, 0);
	lua_pop(mainthread, 1);

	return L;
}

uv_loop_t* luauv_execute_getloop(lua_State* L) {
	return lua_getthreaddata(lua_mainthread(L));
}
