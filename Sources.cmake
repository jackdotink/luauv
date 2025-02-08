target_include_directories(luvu PRIVATE luvu/include)
target_sources(luvu PRIVATE
	luvu/src/main.c

	# runtime core
	luvu/src/runtime.c
	luvu/src/threadref.c

	# globals
	luvu/src/require.c
	luvu/src/lib/task.c

	# loadable libraries
	luvu/src/lib/fs.c

	# misc
	luvu/src/util/string.c
	luvu/src/util/fs.c
)