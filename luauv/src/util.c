#include "util.h"

#include <string.h>
#include <stdlib.h>

#include "lua.h"
#include "uv.h"

int luauv_readfile(const char* filename, char** data, size_t* len) {
	uv_fs_t open_req;
	uv_fs_open(NULL, &open_req, filename, O_RDONLY, 0, NULL);
	if (open_req.result < 0) {
		return open_req.result;
	}

	uv_fs_t read_req;
	uv_buf_t buf = uv_buf_init(malloc(1024), 1024);

	size_t temp_len = 0;
	size_t temp_size = 1024;
	char* temp_data = malloc(1024);

	while (1) {
		uv_fs_read(NULL, &read_req, open_req.result, &buf, 1, temp_len, NULL);
		if (read_req.result < 0) {
			return read_req.result;
		}

		if (read_req.result == 0) {
			break;
		}

		temp_len += read_req.result;
		if (temp_len >= temp_size) {
			temp_size *= 2;
			temp_data = realloc(temp_data, temp_size);
		}

		memcpy(temp_data + temp_len - read_req.result, buf.base, read_req.result);
	}

	free(buf.base);

	*data = realloc(temp_data, temp_len);
	*len = temp_len;

	return 0;
}

void luauv_dumpstack(lua_State* L) {
	int top = lua_gettop(L);
	for (int i = 1; i <= top; i++) {
		printf("%d: ", i);

		int t = lua_type(L, i);
		switch (t) {
			case LUA_TNIL:
				printf("nil");
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TLIGHTUSERDATA:
				printf("lightuserdata");
				break;
			case LUA_TNUMBER:
				printf("number %g", lua_tonumber(L, i));
				break;
			case LUA_TVECTOR:
				{} // this is silly, cmon C
				const float* v = lua_tovector(L, i);
				printf("vector %g %g %g", v[0], v[1], v[2]);
				break;
			case LUA_TSTRING:
				printf("string '%s'", lua_tostring(L, i));
				break;
			case LUA_TTABLE:
				printf("table");
				break;
			case LUA_TFUNCTION:
				printf("function");
				break;
			case LUA_TUSERDATA:
				printf("userdata");
				break;
			case LUA_TTHREAD:
				printf("thread");
				break;
			case LUA_TBUFFER:
				printf("buffer");
				break;
			default:
				printf("unknown");
				break;
		}

		printf("\n");
	}
}
