#pragma once

#include "uv.h"
#include "lua.h"

lua_State* luvu_newruntime(uv_loop_t* loop);
lua_State* luvu_newthread(lua_State* L);
uv_loop_t* luvu_getloop(lua_State* L);

void luvu_spawn(lua_State* L, lua_State* from, int nargs);
void luvu_spawnerror(lua_State* L, lua_State* from, const char* fmt, ...);

lua_State* luvu_execute(
	lua_State* main,
	const char* name,
	const char* code,
	size_t codesize
);
