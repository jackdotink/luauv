#pragma once

#define RESULT(T, E) struct { int isok; union { T ok; E err; }; }

#define OK(R, V) (R) { .isok = 1, .ok = V }
#define ERR(R, V) (R) { .isok = 0, .err = V } 
