#include <stdio.h>

#include "lua.h"
#include "runtime.h"
#include "util/fs.h"
#include "util/string.h"
#include "uv.h"

int main(int argc, char* argv[]) {
	uv_loop_t* loop = uv_default_loop();
	if (loop == NULL) {
		fprintf(stderr, "failed to create uv loop\n");
		return 1;
	}

	lua_State* mainthread = luvu_newruntime(loop);

	fs_readresult result = fs_readfile(cstring_asstr("src/main.luau"));
	if (!result.isok) {
		fprintf(stderr, "failed to read 'src/main.luau': %s\n", cstring_fromstr(result.err));
		return 1;
	}

	luvu_execute(mainthread, "src/main.luau", result.ok.ptr, result.ok.len);
	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}