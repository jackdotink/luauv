#pragma once

#include "util/string.h"
#include "util/result.h"

typedef RESULT(string, str) fs_readresult;
fs_readresult fs_readfile(str path);
