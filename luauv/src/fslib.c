#include "fslib.h"

#include <stdlib.h>
#include <string.h>

#include "execute.h"
#include "lua.h"
#include "lualib.h"
#include "threadref.h"
#include "scheduler.h"
#include "uv.h"

typedef struct {
	uv_fs_t uv;

	luauv_threadref_t thread;

	uv_file fd;
	uv_buf_t buf;

	size_t result_cap;
	size_t result_len;
	char* result_ptr;
} fsread_t;

static void fsread_cleanup(fsread_t* req) {
	uv_fs_req_cleanup(&req->uv);

	if (req->fd >= 0)
		uv_fs_close(NULL, &req->uv, req->fd, NULL);

	if (req->buf.base != NULL)
		free(req->buf.base);

	if (req->result_ptr != NULL)
		free(req->result_ptr);

	luauv_threadref_cleanup(&req->thread);
}

static void fsread_readcb(uv_fs_t* uv) {
	fsread_t* req = uv->data;
	size_t result = uv->result;
	
	lua_State* thread = luauv_threadref_get(&req->thread);

	if (result < 0) {
		luauv_scheduler_spawnerror(thread, NULL, "error while reading '%s': %s", req->uv.path, uv_strerror(uv->result));
	} else if (result > 0) {
		req->result_len += result;
		if (req->result_len > req->result_cap) {
			req->result_cap += req->result_cap / 2;
			req->result_ptr = realloc(req->result_ptr, req->result_cap);
		}

		memcpy(req->result_ptr + req->result_len - result, req->buf.base, result);

		uv_fs_req_cleanup(&req->uv);
		uv_fs_read(
			luauv_execute_getloop(thread),
			&req->uv,
			req->fd,
			&req->buf,
			1,
			-1,
			fsread_readcb
		);
	} else {
		lua_pushlstring(thread, req->result_ptr, req->result_len);
		luauv_scheduler_spawn(thread, NULL, 1);

		fsread_cleanup(req);
		free(req);
	}
}

static void fsread_opencb(uv_fs_t* uv) {
	fsread_t* req = uv->data;
	int result = uv->result;

	lua_State* thread = luauv_threadref_get(&req->thread);

	if (result < 0) {
		luauv_scheduler_spawnerror(thread, NULL, "error while opening '%s': %s", req->uv.path, uv_strerror(result));
	} else {
		req->fd = result;

		req->buf = uv_buf_init(malloc(4096), 4096);
		req->result_cap = 4096;
		req->result_len = 0;
		req->result_ptr = malloc(4096);

		uv_fs_req_cleanup(&req->uv);
		uv_fs_read(
			luauv_execute_getloop(thread),
			&req->uv,
			req->fd,
			&req->buf,
			1,
			0,
			fsread_readcb
		);
	}
}

static int fsread_fn(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);

	fsread_t* req = malloc(sizeof(fsread_t));
	luauv_threadref_init(&req->thread, L);
	
	req->uv.data = req;

	uv_fs_open(
		luauv_execute_getloop(L),
		&req->uv,
		path,
		UV_FS_O_RDONLY,
		0,
		fsread_opencb
	);

	return lua_yield(L, 0);
}

int luauv_fs_open(lua_State* L) {
	lua_createtable(L, 0, 1);

	lua_pushcfunction(L, fsread_fn, "read");
	lua_setfield(L, -2, "read");

	lua_setreadonly(L, -1, 1);

	return 1;
}
