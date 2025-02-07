#include "lib/task.h"

#include <stdlib.h>

#include "lualib.h"
#include "runtime.h"
#include "threadref.h"

static int taskspawn(lua_State* L) {
	lua_State* thread;

	if (lua_isthread(L, 1)) {
		thread = lua_tothread(L, 1);
	} else if (lua_isfunction(L, 1)) {
		thread = luvu_newthread(L);

		lua_xpush(L, thread, 1);
		lua_replace(L, 1);
	} else {
		luaL_typeerror(L, 1, "thread or function");
	}

	int nargs = lua_gettop(L) - 1;

	lua_xmove(L, thread, nargs);
	luvu_spawn(thread, L, nargs);

	return 1;
}

typedef struct {
	uv_timer_t uv;

	luvu_threadref_t thread;
	luvu_threadref_t from;

	int argsref;
} taskdelay_t;

static void taskdelay_cb(uv_timer_t* uv) {
	taskdelay_t* data = uv->data;

	lua_State* thread = luvu_threadref_get(&data->thread);
	lua_State* from = luvu_threadref_get(&data->from);

	lua_getref(thread, data->argsref);
	int nargs = lua_objlen(thread, -1);

	for (int i = 0; i < nargs; i++) {
		lua_rawgeti(thread, -(i + 1), i + 1);
	}

	luvu_spawn(thread, from, nargs);

	uv_timer_stop(uv);
	lua_unref(thread, data->argsref);
	luvu_threadref_cleanup(&data->thread);
	luvu_threadref_cleanup(&data->from);
	free(data);
}

static int taskdelay_fn(lua_State* L) {
	double time = luaL_checknumber(L, 1);
	lua_State* thread;

	if (lua_isthread(L, 2)) {
		thread = lua_tothread(L, 2);
	} else if (lua_isfunction(L, 2)) {
		thread = luvu_newthread(L);

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

	luvu_threadref_init(&req->thread, thread);
	luvu_threadref_init(&req->from, L);
	req->argsref = lua_ref(L, -1);
	
	uv_timer_init(luvu_getloop(L), &req->uv);
	req->uv.data = req;

	uv_timer_start(&req->uv, taskdelay_cb, (uint64_t) (time * 1000.0), 0);

	lua_pop(L, 1);

	return 1;
}

int luvu_opentask(lua_State *L) {
	static const luaL_Reg tasklib[] = {
		{"spawn", taskspawn},
		{"delay", taskdelay_fn},
		{NULL, NULL},
	};

	luaL_register(L, "task", tasklib);

	return 1;
}
