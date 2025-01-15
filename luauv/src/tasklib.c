#include "tasklib.h"

#include <stdlib.h>

#include "execute.h"
#include "lua.h"
#include "lualib.h"
#include "scheduler.h"
#include "threadref.h"
#include "uv.h"

static int taskspawn(lua_State* L) {
	lua_State* thread;

	if (lua_isthread(L, 1)) {
		thread = lua_tothread(L, 1);
	} else if (lua_isfunction(L, 1)) {
		thread = luauv_execute_newthread(L);

		lua_xpush(L, thread, 1);
		lua_replace(L, 1);
	} else {
		luaL_typeerror(L, 1, "thread or function");
	}

	int nargs = lua_gettop(L) - 1;

	lua_xmove(L, thread, nargs);
	luauv_scheduler_spawn(thread, L, nargs);

	return 1;
}

typedef struct {
	luauv_threadref_t thread;
	luauv_threadref_t from;
	int nargs;
} taskdelaydata;

static void taskdelaycb(uv_timer_t* handle) {
	taskdelaydata* data = handle->data;

	luauv_scheduler_spawn(
		luauv_threadref_get(&data->thread),
		luauv_threadref_get(&data->from),
		data->nargs
	);

	luauv_threadref_cleanup(&data->thread);
	luauv_threadref_cleanup(&data->from);
	free(data);

	uv_timer_stop(handle);
	free(handle);
}

static int taskdelay(lua_State* L) {	
	double time = luaL_checknumber(L, 1);
	lua_State* thread;

	if (lua_isthread(L, 2)) {
		thread = lua_tothread(L, 2);
	} else if (lua_isfunction(L, 2)) {
		thread = luauv_execute_newthread(L);

		lua_xpush(L, thread, 2);
		lua_replace(L, 2);
	} else {
		luaL_typeerror(L, 2, "thread or function");
	}

	int nargs = lua_gettop(L) - 2;
	lua_xmove(L, thread, nargs);

	taskdelaydata* data = malloc(sizeof(taskdelaydata));
	luauv_threadref_init(&data->thread, thread);
	luauv_threadref_init(&data->from, L);
	data->nargs = nargs;

	uv_timer_t* handle = malloc(sizeof(uv_timer_t));
	uv_timer_init(luauv_execute_getloop(L), handle);
	handle->data = data;

	uv_timer_start(handle, taskdelaycb, (uint64_t) (time * 1000.0), 0);

	return 1;
}

typedef struct {
	luauv_threadref_t thread;
	uint64_t start;
} taskwaitdata;

static void taskwaitcb(uv_timer_t* handle) {
	taskwaitdata* data = handle->data;
	lua_State* L = luauv_threadref_get(&data->thread);

	double elapsed = (uv_now(luauv_execute_getloop(L)) - data->start) / 1000.0;
	lua_pushnumber(L, elapsed);

	luauv_scheduler_spawn(L, NULL, 1);

	luauv_threadref_cleanup(&data->thread);
	free(data);

	uv_timer_stop(handle);
	free(handle);
}

static int taskwait(lua_State* L) {
	uv_loop_t* loop = luauv_execute_getloop(L);

	double time = luaL_optnumber(L, 1, 0.0);

	taskwaitdata* data = malloc(sizeof(taskwaitdata));
	luauv_threadref_init(&data->thread, L);
	data->start = uv_now(loop);

	uv_timer_t* handle = malloc(sizeof(uv_timer_t));
	uv_timer_init(loop, handle);
	handle->data = data;

	uv_timer_start(handle, taskwaitcb, (uint64_t) (time * 1000.0), 0);

	return lua_yield(L, 0);
}

int luauv_task_open(lua_State* L) {
	static const luaL_Reg tasklib[] = {
		{"spawn", taskspawn},
		{"delay", taskdelay},
		{"wait", taskwait},
		{NULL, NULL},
	};

	luaL_register(L, "task", tasklib);

	return 1;
}
