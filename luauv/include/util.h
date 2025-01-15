#pragma once

#include <stdlib.h>

#include "lua.h"

int luauv_readfile(const char* filename, char** data, size_t* len);
void luauv_dumpstack(lua_State* L);
