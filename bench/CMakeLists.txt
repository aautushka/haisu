project(bench CXX)

include_directories (..)

set(SRC 
  zset_bench
  #  partition_bench
) 

add_executable(bench ${SRC})
target_link_libraries(bench benchmark pthread)
