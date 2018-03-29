set path+=.
set path+=googletest/googletest/include

" ale C++ opts
let cpp_flags  = '-std=c++14 -Wall -I. -Igoogletest/googletest/include/ -Igooglebench/googlebench/include'

let g:ale_cpp_clang_options = cpp_flags
let g:ale_cpp_gcc_options = cpp_flags
