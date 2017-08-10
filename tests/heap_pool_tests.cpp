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
#include "haisu/object_pool.h"

struct heap_pool_test : ::testing::Test
{
    haisu::heap_pool<int> pool;
};

TEST_F(heap_pool_test, allocates_from_pool)
{
    int* p = pool.alloc();
    EXPECT_TRUE(p != nullptr);
}
TEST_F(heap_pool_test, allocates_same_object_once_freed)
{
    auto p1 = pool.alloc();
    pool.dealloc(p1);
    auto p2 = pool.alloc();
    EXPECT_EQ(p1, p2);
}

TEST_F(heap_pool_test, allocates_object_at_different_addr)
{
    int* p1 = pool.alloc();
    int* p2 = pool.alloc();

    EXPECT_NE(p1, p2);
}

TEST_F(heap_pool_test, initilly_has_zero_size_and_zero_capacity)
{
    EXPECT_EQ(0, pool.capacity());
    EXPECT_EQ(0, pool.size());
}

TEST_F(heap_pool_test, size_is_increased_as_a_result_of_object_allocation)
{
    pool.alloc();
    EXPECT_EQ(1, pool.size());
    EXPECT_GE(pool.capacity(), pool.size());
    
    pool.alloc();
    EXPECT_EQ(2, pool.size());
    EXPECT_GE(pool.capacity(), pool.size());

    pool.alloc();
    EXPECT_EQ(3, pool.size());
    EXPECT_GE(pool.capacity(), pool.size());

    pool.alloc();
    EXPECT_EQ(4, pool.size());
    EXPECT_GE(pool.capacity(), pool.size());
}

TEST_F(heap_pool_test, constructs_object)
{
    haisu::heap_pool<std::string> pool;
    auto s = pool.construct("hello world");
    EXPECT_EQ("hello world", *s);
    pool.destroy(s);
}

TEST_F(heap_pool_test, size_is_decreased_once_the_object_gets_freed)
{
    auto p = pool.alloc();
    pool.dealloc(p);

    EXPECT_EQ(0, pool.size());
}

TEST_F(heap_pool_test, deallocs_all_objects_without_calling_the_descructors)
{
    auto p = pool.alloc();
    pool.dealloc_all();

    EXPECT_EQ(0, pool.size());
}

TEST_F(heap_pool_test, reuses_deallocated_memory)
{
    for (int i = 0; i < 2; ++i)
    {
        pool.alloc();
    }
    pool.dealloc_all();

    auto prev_capacity = pool.capacity();
    for (size_t i = 0; i < prev_capacity; ++i)
    {
        pool.alloc();
        EXPECT_EQ(prev_capacity, pool.capacity());
    }
}

