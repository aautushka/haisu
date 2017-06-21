cmake_minimum_required(VERSION 3.0)

project(haisu)

add_compile_options(-std=gnu++14)

add_subdirectory(haisu)
add_subdirectory(tests)
add_subdirectory(bench)
