project(bench CXX)

include_directories (..)

set(SRC 
  main.cpp
  # zset_bench
  # partition_bench
  # mono_hash_bench
  # metric_bench
  json_bench
  # js0n/js0n.h
  # js0n/js0n.c
) 

add_subdirectory(simdjson)

add_executable(bench ${SRC})
target_link_libraries(bench benchmark pthread simdjson)
