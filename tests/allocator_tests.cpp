/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
#include <gtest/gtest.h>
#include "haisu/memory.h"

struct allocator_test : ::testing::Test
{
    using mem_t = haisu::podbump;
    using allocator_t = haisu::allocator<int, mem_t>;

    mem_t mem;
    allocator_t allocator;

    allocator_test()
        : allocator(mem)
    {
    }
};

TEST_F(allocator_test, allocates_memory)
{
    int* p = allocator.allocate(1);
    EXPECT_TRUE(p != nullptr);
}

TEST_F(allocator_test, integrates_with_standard_containers)
{
    std::vector<int, allocator_t> vec(allocator);
    vec.push_back(0xdeadbeef);
    vec.push_back(0xbaddcafe);

    EXPECT_EQ(0xdeadbeef, vec[0]);
    EXPECT_EQ(0xbaddcafe, vec[1]);
}
