#pragma once

#include <stdlib.h>

// A null-terminated owning immutable string.
typedef const char* cstring_t;

// An owning mutable string
typedef struct {
	size_t cap;
	size_t len;
	char* ptr;
} string_t;

// A non-owning immutable string.
typedef struct {
	const size_t len;
	const char* ptr;
} str_t;

cstring_t cstring_fromstring(string_t* s);
cstring_t cstring_fromstr(str_t s);

string_t string_new();
string_t string_clone(string_t* s);
string_t string_fromstr(str_t s);
string_t string_fromcstring(cstring_t s);

void string_init(string_t* s);

void string_cleanup(string_t* s);
cstring_t string_intocstring(string_t* s);

void string_setsize(string_t* s, size_t sz);
void string_reserve(string_t* s, size_t sz);
void string_shrink(string_t* s);

void string_pushchar(string_t* s, char c);
void string_pushstr(string_t* s, str_t other);
void string_append(string_t* s, string_t* other);

str_t string_slice(string_t* s, size_t start, size_t len);
str_t string_asstr(string_t* s);

str_t cstring_slice(cstring_t s, size_t start, size_t len);
str_t cstring_asstr(cstring_t s);

str_t str_slice(str_t s, size_t start, size_t len);
