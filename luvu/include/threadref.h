#pragma once

#include "lua.h"

typedef struct {
	lua_State* GL;
	int refid;
} luvu_threadref_t;

void luvu_threadref_init(luvu_threadref_t* ref, lua_State* L);
void luvu_threadref_cleanup(luvu_threadref_t* ref);

void luvu_threadref_push(luvu_threadref_t* ref, lua_State* L);
lua_State* luvu_threadref_get(luvu_threadref_t* ref);
