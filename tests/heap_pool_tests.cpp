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
#include "haisu/object_pool.h"

struct heap_pool_test : ::testing::Test
{
    using pool_t = haisu::heap_pool<int>;
    pool_t pool;

    void alloc_many(int n) {
        for (int i = 0; i < n; ++i) {
            pool.alloc();
        }
    }
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

TEST_F(heap_pool_test, allocates_same_object_if_freed_all)
{
    auto p1 = pool.alloc();
    pool.dealloc_all();
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

TEST_F(heap_pool_test, keeps_memory_after_deallocation) {
    alloc_many(10);
    auto prev_capacity = pool.capacity();
    pool.dealloc_all();
    EXPECT_EQ(prev_capacity, pool.capacity());
}

TEST_F(heap_pool_test, reuses_deallocated_memory)
{
    alloc_many(100);
    pool.dealloc_all();

    auto prev_capacity = pool.capacity();
    for (size_t i = 0; i < prev_capacity; ++i)
    {
        pool.alloc();
        EXPECT_EQ(prev_capacity, pool.capacity());
    }
}

TEST_F(heap_pool_test, moves_object) {
    alloc_many(10);
    pool_t other(std::move(pool));
    pool = std::move(other);
}

