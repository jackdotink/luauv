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
	uv_timer_t uv;

	luauv_threadref_t thread;
	luauv_threadref_t from;
	int argsref;
} taskdelay_t;

static void taskdelay_cb(uv_timer_t* uv) {
	taskdelay_t* req = uv->data;

	lua_State* thread = luauv_threadref_get(&req->thread);
	lua_State* from = luauv_threadref_get(&req->from);

	lua_getref(thread, req->argsref);
	int nargs = lua_objlen(thread, -1);
	
	for (int i = 0; i < nargs; i++) {
		lua_rawgeti(thread, -(i + 1), i + 1);
	}

	luauv_scheduler_spawn(thread, from, nargs);

	uv_timer_stop(uv);
	luauv_threadref_cleanup(&req->thread);
	luauv_threadref_cleanup(&req->from);
	lua_unref(thread, req->argsref);
	free(req);
}

static int taskdelay_fn(lua_State* L) {
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
	lua_createtable(L, nargs, 0);

	for (int i = 0; i < nargs; i++) {
		lua_pushvalue(L, i + 3);
		lua_rawseti(L, -2, i + 1);
	}

	taskdelay_t* req = malloc(sizeof(taskdelay_t));

	luauv_threadref_init(&req->thread, thread);
	luauv_threadref_init(&req->from, L);
	req->argsref = lua_ref(L, -1);
	
	uv_timer_init(luauv_execute_getloop(L), &req->uv);
	req->uv.data = req;

	uv_timer_start(&req->uv, taskdelay_cb, (uint64_t) (time * 1000.0), 0);

	lua_pop(L, 1);

	return 1;
}

typedef struct {
	uv_timer_t uv;

	luauv_threadref_t thread;
	double start;
} taskwait_t;

static void taskwait_cb(uv_timer_t* uv) {
	taskwait_t* req = uv->data;

	lua_State* thread = luauv_threadref_get(&req->thread);

	lua_pushnumber(thread, lua_clock() - req->start);
	luauv_scheduler_spawn(thread, thread, 1);

	uv_timer_stop(uv);
	luauv_threadref_cleanup(&req->thread);
	free(req);
}

static int taskwait_fn(lua_State* L) {
	double time = luaL_checknumber(L, 1);
	
	taskwait_t* req = malloc(sizeof(taskwait_t));

	luauv_threadref_init(&req->thread, L);
	req->start = lua_clock();

	uv_timer_init(luauv_execute_getloop(L), &req->uv);
	req->uv.data = req;

	uv_timer_start(&req->uv, taskwait_cb, (uint64_t) (time * 1000.0), 0);

	return lua_yield(L, 0);
}

int luauv_task_open(lua_State* L) {
	static const luaL_Reg tasklib[] = {
		{"spawn", taskspawn},
		{"delay", taskdelay_fn},
		{"wait", taskwait_fn},
		{NULL, NULL},
	};

	luaL_register(L, "task", tasklib);

	return 1;
}
