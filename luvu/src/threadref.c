#include "threadref.h"

#include "lua.h"

void luvu_threadref_init(luvu_threadref_t* ref, lua_State* L) {
	ref->GL = lua_mainthread(L);
	
	lua_pushthread(L);
	ref->refid = lua_ref(L, -1);
	lua_pop(L, 1);
}

void luvu_threadref_cleanup(luvu_threadref_t* ref) {
	lua_unref(ref->GL, ref->refid);
}

void luvu_threadref_push(luvu_threadref_t* ref, lua_State* L) {
	lua_getref(L, ref->refid);
}

lua_State* luvu_threadref_get(luvu_threadref_t* ref) {
	lua_State* globalstate = ref->GL;
	luvu_threadref_push(ref, globalstate);
	lua_State* thread = lua_tothread(globalstate, -1);
	lua_pop(globalstate, 1);
	return thread;
}
