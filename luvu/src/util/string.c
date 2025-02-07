#include "util/string.h"

#include <string.h>

cstring cstring_fromstring(string* s) {
	char* ptr = malloc(s->len + 1);
	memcpy(ptr, s->ptr, s->len);
	ptr[s->len] = 0;

	return ptr;
}

cstring cstring_fromstr(str s) {
	char* ptr = malloc(s.len + 1);
	memcpy(ptr, s.ptr, s.len);
	ptr[s.len] = 0;

	return ptr;
}

string string_new() {
	return (string) {
		.cap = 16,
		.len = 0,
		.ptr = malloc(16),
	};
}

string string_clone(string* s) {
	size_t len = s->len;
	char* ptr = malloc(len);
	memcpy(ptr, s->ptr, len);

	return (string) {
		.cap = len,
		.len = len,
		.ptr = ptr,
	};
}

string string_fromstr(str s) {
	char* ptr = malloc(s.len);
	memcpy(ptr, s.ptr, s.len);

	return (string) {
		.cap = s.len,
		.len = s.len,
		.ptr = ptr,
	};
}

string string_fromcstring(cstring s) {
	size_t len = strlen(s);
	char* ptr = malloc(len);
	memcpy(ptr, s, len);

	return (string) {
		.cap = len,
		.len = len,
		.ptr = ptr,
	};
}

void string_cleanup(string* s) {
	free(s->ptr);
}

cstring string_intocstring(string* s) {
	string_setsize(s, s->len + 1);
	s->ptr[s->len] = 0;
	return s->ptr;
}

void string_setsize(string* s, size_t sz) {
	s->cap = sz;
	s->ptr = realloc(s->ptr, sz);

	if (s->len > sz)
		s->len = sz;
}

void string_reserve(string* s, size_t sz) {
	if (s->cap >= sz)
		return;

	do {
		s->cap += s->cap / 2;
	} while (s->cap < sz);

	s->ptr = realloc(s->ptr, s->cap);
}

void string_shrink(string* s) {
	if (s->cap == s->len)
		return;

	s->cap = s->len;
	s->ptr = realloc(s->ptr, s->cap);
}

void string_pushchar(string* s, char c) {
	string_reserve(s, s->len + 1);
	s->ptr[s->len] = c;
	s->len += 1;
}

void string_pushstr(string* s, str other) {
	string_reserve(s, s->len + other.len);
	memcpy(s->ptr + s->len, other.ptr, other.len);
	s->len += other.len;
}

void string_append(string* s, string* other) {
	string_reserve(s, s->len + other->len);
	memcpy(s->ptr + s->len, other->ptr, other->len);
	s->len += other->len;
}

str string_slice(string* s, size_t start, size_t len) {
	return (str) {
		.len = len,
		.ptr = s->ptr + start,
	};
}

str string_asstr(string* s) {
	return (str) {
		.len = s->len,
		.ptr = s->ptr,
	};
}

str cstring_slice(cstring s, size_t start, size_t len) {
	return (str) {
		.len = len,
		.ptr = s,
	};
}

str cstring_asstr(cstring s) {
	return (str) {
		.len = strlen(s),
		.ptr = s,
	};
}

str str_slice(str s, size_t start, size_t len) {
	return (str) {
		.len = len,
		.ptr = s.ptr + start,
	};
}
