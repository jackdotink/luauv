#include "fslib.h"

#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "runtime.h"
#include "uv.h"

typedef struct {
	lua_State* L;
	
	char* filename;
	uv_buf_t buf;

	size_t result_size;
	size_t result_len;
	char* result;
} fsread_data_t;

static fsread_data_t* fsread_data_init(lua_State* L, const char* filename) {
	fsread_data_t* data = malloc(sizeof(fsread_data_t));

	data->L = L;
	
	data->filename = malloc(strlen(filename) + 1);
	strcpy(data->filename, filename);

	data->buf = uv_buf_init(malloc(1024), 1024);

	data->result_size = 1024;
	data->result_len = 0;
	data->result = malloc(1024);

	return data;
}

static void fsread_data_cleanup(fsread_data_t* data) {
	free(data->filename);
	free(data->buf.base);
	free(data->result);
	free(data);
}

static void fsreadtostring_readcb(uv_fs_t* req) {
	fsread_data_t* data = req->data;
	ssize_t nread = req->result;	
	
	lua_State* L = data->L;
	luauv_runtime_t* runtime = luauv_runtime_get(L);

	if (nread < 0) {
		luauv_runtime_spawnerror(runtime, L, NULL, "error reading file '%s': %s", data->filename, uv_strerror(nread));

		fsread_data_cleanup(data);
		free(req);
	} else if (nread > 0) {
		data->result_len += nread;
		if (data->result_len > data->result_size) {
			data->result_size *= 2;
			data->result = realloc(data->result, data->result_size);
		}

		memcpy(data->result + data->result_len - nread, data->buf.base, nread);
		uv_fs_read(runtime->loop, req, req->file, &data->buf, 1, -1, fsreadtostring_readcb);
	} else {
		lua_pushlstring(L, data->result, data->result_len);
		uv_fs_close(runtime->loop, req, req->file, NULL);
		
		fsread_data_cleanup(data);
		free(req);

		luauv_runtime_spawn(runtime, L, NULL, 1);
	}
}

static void fsreadtostring_opencb(uv_fs_t* req) {
	fsread_data_t* data = req->data;
	ssize_t fd = req->result;

	lua_State* L = data->L;
	luauv_runtime_t* runtime = luauv_runtime_get(L);

	if (fd < 0) {
		luauv_runtime_spawnerror(runtime, L, NULL, "error opening file '%s': %s", data->filename, uv_strerror(fd));

		fsread_data_cleanup(data);
		free(req);
	} else {
		uv_fs_read(runtime->loop, req, fd, &data->buf, 1, -1, fsreadtostring_readcb);
	}
}

static int fsreadtostring(lua_State* L) {
	luauv_runtime_t* runtime = luauv_runtime_get(L);

	fsread_data_t* data = fsread_data_init(L, luaL_checkstring(L, 1));
	uv_fs_t* req = malloc(sizeof(uv_fs_t));
	req->data = data;

	uv_fs_open(runtime->loop, req, data->filename, O_RDONLY, 0, fsreadtostring_opencb);

	return lua_yield(L, 0);
}

int luauv_openfs(lua_State* L) {
	lua_createtable(L, 0, 1);

	lua_pushcfunction(L, fsreadtostring, "readtostring");
	lua_setfield(L, -2, "readtostring");

	return 1;
}
