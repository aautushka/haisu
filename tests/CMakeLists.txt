project(tests CXX)

enable_testing()
find_package(GTest REQUIRED)
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
  mono_string_tests
  memrmem_tests
  mono_list_tests
  mono_slist_tests
  mono_xorlist_tests
  synchronized_tests
  mono_hash_tests
  json_tests
  json_bitstack_tests
  object_pool_tests
  heterogeneous_pool_tests
  small_any_tests
  function_ref_tests
  json_model_tests
  trivial_variant_tests
  heap_pool_tests
)

add_executable(tests ${SRC})

target_link_libraries(tests gtest pthread)
