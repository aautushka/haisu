/*
MIT License

Copyright (c) 2017 Anton Autushka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
