#include "util/fs.h"

#include "uv.h"

fs_readresult fs_readfile(str path) {
	uv_fs_t openreq;
	uv_fs_open(NULL, &openreq, cstring_fromstr(path), UV_FS_O_RDONLY, 0, NULL);

	int fd = openreq.result;
	if (fd < 0) {
		return ERR(fs_readresult, cstring_asstr(uv_strerror(fd)));
	}

	uv_buf_t buf = uv_buf_init(malloc(1024), 1024);
	string data = string_new();

	uv_fs_t readreq;

	while (1) {
		int read = uv_fs_read(NULL, &readreq, fd, &buf, 1, -1, NULL);

		if (read < 0) {
			return ERR(fs_readresult, cstring_asstr(uv_strerror(fd)));
		} else if (read == 0) {
			break;
		}

		string_pushstr(&data, (str) {
			.len = buf.len,
			.ptr = buf.base,
		});
	}

	return OK(fs_readresult, data);
}
