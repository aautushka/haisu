project(tests CXX)

enable_testing()
#find_package(GTest REQUIRED)
include_directories (..)

set(SRC 
  zmap_tests.cpp
  zbuf_tests.cpp
  binary_search_tests.cpp
  linear_hash_tests.cpp
  tls_tests.cpp
  podbump_tests.cpp
  allocator_tests.cpp
  intrusive_list_tests.cpp
  list_tests.cpp
  growbump_tests.cpp
  refbump_tests.cpp
  mono_stack_tests.cpp
  overflow_stack_tests.cpp
  tree_tests.cpp
  metric_table_tests.cpp
  metric_monitor_tests.cpp
  metric_monitor2_tests.cpp
  mono_queue_tests.cpp
  mono_string_tests.cpp
  memrmem_tests.cpp
  mono_list_tests.cpp
  mono_slist_tests.cpp
  mono_xorlist_tests.cpp
  synchronized_tests.cpp
  mono_hash_tests.cpp
  json_tests.cpp
  json_bitstack_tests.cpp
  object_pool_tests.cpp
  heterogeneous_pool_tests.cpp
  small_any_tests.cpp
  function_ref_tests.cpp
  json_model_tests.cpp
  trivial_variant_tests.cpp
  heap_pool_tests.cpp
  metric_trie_tests.cpp
)

add_executable(tests ${SRC})

target_link_libraries(tests gtest pthread)
