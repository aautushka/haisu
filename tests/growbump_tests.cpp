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
