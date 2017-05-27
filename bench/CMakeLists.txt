project(bench CXX)

include_directories (..)

set(SRC 
  main.cpp
  # zset_bench
  # partition_bench
  # mono_hash_bench
  json_bench
) 

add_executable(bench ${SRC})
target_link_libraries(bench benchmark pthread)
