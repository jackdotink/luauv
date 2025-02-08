#pragma once

#include <stdbool.h>

#define RESULT(T, E) struct { bool isok; union { T ok; E err; }; }

#define OK(R, V) (R) { .isok = true, .ok = V }
#define ERR(R, V) (R) { .isok = false, .err = V } 
