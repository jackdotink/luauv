#include "util/string.h"

#include <string.h>

cstring_t cstring_fromstring(string_t* s) {
	char* ptr = malloc(s->len + 1);
	memcpy(ptr, s->ptr, s->len);
	ptr[s->len] = 0;

	return ptr;
}

cstring_t cstring_fromstr(str_t s) {
	char* ptr = malloc(s.len + 1);
	memcpy(ptr, s.ptr, s.len);
	ptr[s.len] = 0;

	return ptr;
}

string_t string_new() {
	return (string_t) {
		.cap = 16,
		.len = 0,
		.ptr = malloc(16),
	};
}

string_t string_clone(string_t* s) {
	size_t len = s->len;
	char* ptr = malloc(len);
	memcpy(ptr, s->ptr, len);

	return (string_t) {
		.cap = len,
		.len = len,
		.ptr = ptr,
	};
}

string_t string_fromstr(str_t s) {
	char* ptr = malloc(s.len);
	memcpy(ptr, s.ptr, s.len);

	return (string_t) {
		.cap = s.len,
		.len = s.len,
		.ptr = ptr,
	};
}

string_t string_fromcstring(cstring_t s) {
	size_t len = strlen(s);
	char* ptr = malloc(len);
	memcpy(ptr, s, len);

	return (string_t) {
		.cap = len,
		.len = len,
		.ptr = ptr,
	};
}

void string_init(string_t *s) {
	s->cap = 16;
	s->len = 0;
	s->ptr = malloc(16);
}

void string_cleanup(string_t* s) {
	free(s->ptr);
}

cstring_t string_intocstring(string_t* s) {
	string_setsize(s, s->len + 1);
	s->ptr[s->len] = 0;
	return s->ptr;
}

void string_setsize(string_t* s, size_t sz) {
	s->cap = sz;
	s->ptr = realloc(s->ptr, sz);

	if (s->len > sz)
		s->len = sz;
}

void string_reserve(string_t* s, size_t sz) {
	if (s->cap >= sz)
		return;

	do {
		s->cap += s->cap / 2;
	} while (s->cap < sz);

	s->ptr = realloc(s->ptr, s->cap);
}

void string_shrink(string_t* s) {
	if (s->cap == s->len)
		return;

	s->cap = s->len;
	s->ptr = realloc(s->ptr, s->cap);
}

void string_pushchar(string_t* s, char c) {
	string_reserve(s, s->len + 1);
	s->ptr[s->len] = c;
	s->len += 1;
}

void string_pushstr(string_t* s, str_t other) {
	string_reserve(s, s->len + other.len);
	memcpy(s->ptr + s->len, other.ptr, other.len);
	s->len += other.len;
}

void string_append(string_t* s, string_t* other) {
	string_reserve(s, s->len + other->len);
	memcpy(s->ptr + s->len, other->ptr, other->len);
	s->len += other->len;
}

str_t string_slice(string_t* s, size_t start, size_t len) {
	return (str_t) {
		.len = len,
		.ptr = s->ptr + start,
	};
}

str_t string_asstr(string_t* s) {
	return (str_t) {
		.len = s->len,
		.ptr = s->ptr,
	};
}

str_t cstring_slice(cstring_t s, size_t start, size_t len) {
	return (str_t) {
		.len = len,
		.ptr = s,
	};
}

str_t cstring_asstr(cstring_t s) {
	return (str_t) {
		.len = strlen(s),
		.ptr = s,
	};
}

str_t str_slice(str_t s, size_t start, size_t len) {
	return (str_t) {
		.len = len,
		.ptr = s.ptr + start,
	};
}
