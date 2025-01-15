#pragma once

#include "lua.h"

void luauv_scheduler_spawn(lua_State* thread, lua_State* from, int nargs);
void luauv_scheduler_spawnerror(lua_State* thread, lua_State* from, const char* fmt, ...);
