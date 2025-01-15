#include "threadref.h"

#include "lua.h"

void luauv_threadref_init(luauv_threadref_t* ref, lua_State* L) {
	ref->globalstate = lua_mainthread(L);
	
	lua_pushthread(L);
	ref->refid = lua_ref(L, -1);
	lua_pop(L, 1);
}

void luauv_threadref_cleanup(luauv_threadref_t* ref) {
	lua_unref(ref->globalstate, ref->refid);
}

void luauv_threadref_push(luauv_threadref_t *ref, lua_State *L) {
	lua_getref(L, ref->refid);
}

lua_State* luauv_threadref_get(luauv_threadref_t* ref) {
	lua_State* globalstate = ref->globalstate;
	luauv_threadref_push(ref, globalstate);
	lua_State* thread = lua_tothread(globalstate, -1);
	lua_pop(globalstate, 1);
	return thread;
}
