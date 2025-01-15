#include "scheduler.h"

#include <stdlib.h>
#include <stdio.h>

#include "lua.h"

static void handlestatus(lua_State* thread, int status) {
	if (status != LUA_OK && status != LUA_YIELD) {
		const char* error = lua_tostring(thread, -1);
		if (error == NULL) {
			error = "unknown error";
		}

		const char* trace = lua_debugtrace(thread);

		fprintf(stderr, "%s\ntrace:\n%s", error, trace);
	}
}

void luauv_scheduler_spawn(lua_State* thread, lua_State* from, int nargs) {
	int status = lua_resume(thread, from, nargs);
	handlestatus(thread, status);
}

void luauv_scheduler_spawnerror(lua_State* thread, lua_State* from, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	lua_pushvfstring(thread, fmt, args);
	va_end(args);

	int status = lua_resumeerror(thread, from);
	handlestatus(thread, status);
}
