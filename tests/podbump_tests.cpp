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

struct podbump_test : ::testing::Test
{
    haisu::podbump pod;
};

TEST_F(podbump_test, allocates_memory)
{
    int* p = pod.alloc<int>();
    EXPECT_TRUE(p != nullptr);
}

TEST_F(podbump_test, new_allocation_goes_to_different_address)
{
    int* p1 = pod.alloc<int>();
    int* p2 = pod.alloc<int>();

    EXPECT_TRUE(p1 != p2);
}

TEST_F(podbump_test, subsequent_allocation_do_not_interfere_with_one_another)
{
    int* p1 = pod.alloc<int>();
    int* p2 = pod.alloc<int>();

    *p1 = 0xdeadbeef;
    *p2 = 0xbaddcafe;

    EXPECT_EQ(0xdeadbeef, *p1);
    EXPECT_EQ(0xbaddcafe, *p2);
}

TEST_F(podbump_test, return_null_if_memory_was_exhausted)
{
    haisu::podbump pod(sizeof(int));
    int* p = pod.alloc<int>();
    ASSERT_TRUE(p != nullptr);

    EXPECT_EQ(nullptr, pod.alloc<int>());
}

TEST_F(podbump_test, recycles_memory)
{
    haisu::podbump pod(sizeof(int));
    int* p = pod.alloc<int>();
    pod.free(p);

    EXPECT_NE(nullptr, pod.alloc<int>());
}
