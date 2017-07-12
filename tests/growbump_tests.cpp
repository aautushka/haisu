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

struct growbump_test : ::testing::Test
{
    haisu::growbump memory;
};

TEST_F(growbump_test, allocates_memory)
{
    int* p = memory.alloc<int>();
    EXPECT_TRUE(p != nullptr);
}

TEST_F(growbump_test, new_allocation_goes_to_different_address)
{
    int* p1 = memory.alloc<int>();
    int* p2 = memory.alloc<int>();

    EXPECT_NE(nullptr, p1);
    EXPECT_NE(nullptr, p2);

    EXPECT_TRUE(p1 != p2);
}

TEST_F(growbump_test, subsequent_allocation_do_not_interfere_with_one_another)
{
    int* p1 = memory.alloc<int>();
    int* p2 = memory.alloc<int>();

    *p1 = 0xdeadbeef;
    *p2 = 0xbaddcafe;

    EXPECT_EQ(0xdeadbeef, *p1);
    EXPECT_EQ(0xbaddcafe, *p2);
}

TEST_F(growbump_test, automatically_adds_more_memory_if_needed)
{
    for (int i = 0; i < 100000; ++i)
    {
        void* p = memory.alloc(32);
        EXPECT_TRUE(nullptr != p);
    }
}

TEST_F(growbump_test, allocates_huge_buffer_all_at_once)
{
    void* p = memory.alloc(100 * 1024 * 1024);
    EXPECT_TRUE(nullptr != p);
}

TEST_F(growbump_test, allocates_memory_of_any_imaginable_size)
{
    for (int i = 1; i < 128 * 1024 * 1024; i *= 2)
    {
        haisu::growbump memory;
        void* p = memory.alloc(i);
        EXPECT_TRUE(nullptr != p);
    }
}
