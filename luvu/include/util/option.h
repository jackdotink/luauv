#pragma once

#include <stdbool.h>

#define OPTION(T) struct { bool some; T value; }

#define SOME(O, V) O { .some = true, .value = V }
#define NONE(O) O { .some = false }