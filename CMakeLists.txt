cmake_minimum_required(VERSION 3.10)

project(Luauv LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

set(LUAU_BUILD_CLI OFF)
set(LUAU_BUILD_WEB OFF)
set(LUAU_BUILD_TESTS OFF)
add_subdirectory(vendor/luau EXCLUDE_FROM_ALL)

target_compile_definitions(Luau.VM PRIVATE LUA_USE_LONGJMP=1 LUA_API=extern\"C\")
target_compile_definitions(Luau.Compiler PRIVATE LUACODE_API=extern\"C\")
target_compile_definitions(Luau.CodeGen PRIVATE LUACODEGEN_API=extern\"C\")

set(LIBUV_BUILD_TESTS OFF)
set(LIBUV_BUILD_BENCH OFF)
add_subdirectory(vendor/libuv EXCLUDE_FROM_ALL)

add_executable(luauv)

target_compile_definitions(luauv PRIVATE LUA_API= LUACODE_API= LUACODEGEN_API=)

target_compile_features(luauv PRIVATE c_std_11)
target_compile_options(luauv PRIVATE -Wall -Werror)
set_target_properties(luauv PROPERTIES OUTPUT_NAME cli)
target_link_libraries(luauv Luau.Compiler Luau.CodeGen Luau.VM uv_a)

include(Sources.cmake)
