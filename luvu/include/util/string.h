#pragma once

#include <stdlib.h>

// A null-terminated owning immutable string.
typedef const char* cstring;

// An owning mutable string
typedef struct {
	size_t cap;
	size_t len;
	char* ptr;
} string;

// A non-owning immutable string.
typedef struct {
	const size_t len;
	const char* ptr;
} str;

cstring cstring_fromstring(string* s);
cstring cstring_fromstr(str s);

string string_new();
string string_clone(string* s);
string string_fromstr(str s);
string string_fromcstring(cstring s);

void string_cleanup(string* s);
cstring string_intocstring(string* s);

void string_setsize(string* s, size_t sz);
void string_reserve(string* s, size_t sz);
void string_shrink(string* s);

void string_pushchar(string* s, char c);
void string_pushstr(string* s, str other);
void string_append(string* s, string* other);

str string_slice(string* s, size_t start, size_t len);
str string_asstr(string* s);

str cstring_slice(cstring s, size_t start, size_t len);
str cstring_asstr(cstring s);

str str_slice(str s, size_t start, size_t len);
