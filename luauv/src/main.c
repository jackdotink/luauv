#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "execute.h"
#include "lua.h"
#include "util.h"
#include "uv.h"

int main() {
	uv_loop_t* loop = uv_default_loop();
	if (loop == NULL) {
		fprintf(stderr, "failed to create uv loop\n");
		return 1;
	}

	lua_State* mainthread = luauv_execute_newmainthread(loop);

	char* code;
	size_t len;

	{
		int result = luauv_readfile("src/main.luau", &code, &len);
		if (result < 0) {
			fprintf(stderr, "failed to read file 'src/main.luau': %s\n", uv_strerror(result));
			return 1;
		}
	}

	luauv_execute_runfile(mainthread, "src/main.luau", code, len);
	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}