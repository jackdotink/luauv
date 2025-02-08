#pragma once

#include "util/string.h"
#include "util/result.h"

typedef RESULT(string_t, cstring_t) fs_readresult_t;
fs_readresult_t fs_readfile(str_t path);
