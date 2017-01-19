project(tests CXX)

enable_testing()
include_directories (..)

set(SRC 
  zmap_tests 
  zbuf_tests
  binary_search_tests
)

add_executable(tests ${SRC})
target_link_libraries(tests gtest gtest_main pthread)
