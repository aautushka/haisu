cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)
project(haisu LANGUAGES CXX)

# include(googletest.cmake)

add_compile_options(-std=gnu++1z)

add_subdirectory(haisu)

if (${ENABLE_BENCHING}) 
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(googlebench)
    add_subdirectory(bench)
endif()

if (${ENABLE_TESTING}) 
    add_subdirectory(tests)
endif()

