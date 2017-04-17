project(tests CXX)

enable_testing()
include_directories (..)

set(SRC 
  zmap_tests 
  zbuf_tests
  binary_search_tests
  linear_hash_tests
  tls_tests
  podbump_tests
  allocator_tests
  intrusive_list_tests
  list_tests
  growbump_tests
  refbump_tests
  mono_stack_tests
  overflow_stack_tests
  tree_tests
  metric_table_tests
  metric_monitor_tests
  mono_queue_tests
)

add_executable(tests ${SRC})
target_link_libraries(tests gtest gtest_main pthread)
