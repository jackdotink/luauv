#pragma once

#include "uv.h"
#include "lua.h"

lua_State* luauv_execute_newmainthread(uv_loop_t* loop);
lua_State* luauv_execute_newthread(lua_State* L);
uv_loop_t* luauv_execute_getloop(lua_State* L);

lua_State* luauv_execute_runfile(
	lua_State* mainthread,
	const char* filename,
	const char* code,
	size_t codesize
);
