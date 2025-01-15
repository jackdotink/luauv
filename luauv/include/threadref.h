#pragma once

#include "lua.h"

typedef struct {
	lua_State* globalstate;
	int refid;
} luauv_threadref_t;

void luauv_threadref_init(luauv_threadref_t* ref, lua_State* L);
void luauv_threadref_cleanup(luauv_threadref_t* ref);

void luauv_threadref_push(luauv_threadref_t* ref, lua_State* L);
lua_State* luauv_threadref_get(luauv_threadref_t* ref);
