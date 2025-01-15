target_include_directories(luauv PRIVATE luauv/include)
target_sources(luauv PRIVATE
	luauv/src/main.c

	# runtime core
	luauv/src/execute.c
	luauv/src/threadref.c
	luauv/src/scheduler.c

	# globals
	luauv/src/require.c
	luauv/src/tasklib.c

	# loadable libraries

	# misc
	luauv/src/util.c
)