cmake_minimum_required(VERSION 3.1)

project(haisu)
include(googletest.cmake)

add_compile_options(-std=gnu++1z)

set (BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(googlebench)
add_subdirectory(haisu)
add_subdirectory(tests)
add_subdirectory(bench)
