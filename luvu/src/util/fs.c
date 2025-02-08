#include "util/fs.h"

#include "uv.h"

fs_readresult_t fs_readfile(str_t path) {
	uv_fs_t openreq;
	cstring_t cpath = cstring_fromstr(path);
	uv_fs_open(NULL, &openreq, cpath, UV_FS_O_RDONLY, 0, NULL);
	free((void*) cpath);

	int fd = openreq.result;
	if (fd < 0) {
		return ERR(fs_readresult_t, uv_strerror(fd));
	}

	uv_buf_t buf = uv_buf_init(malloc(4096), 4096);
	string_t data = string_new();

	uv_fs_t readreq;

	while (1) {
		int read = uv_fs_read(NULL, &readreq, fd, &buf, 1, -1, NULL);

		if (read < 0) {
			free(buf.base);
			string_cleanup(&data);

			uv_fs_t closereq;
			uv_fs_close(NULL, &closereq, fd, NULL);

			return ERR(fs_readresult_t, uv_strerror(fd));
		} else if (read == 0) {
			break;
		}

		string_pushstr(&data, (str_t) {
			.len = read,
			.ptr = buf.base,
		});
	}

	free(buf.base);

	uv_fs_t closereq;
	uv_fs_close(NULL, &closereq, fd, NULL);

	return OK(fs_readresult_t, data);
}
