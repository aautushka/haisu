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
#include "haisu/heterogeneous_pool.h"

struct heterogeneous_pool_test : ::testing::Test
{
    haisu::heterogeneous_pool<10, int> pool;
};

TEST_F(heterogeneous_pool_test, allocates_from_pool)
{
    int* p = pool.alloc<int>();
    EXPECT_TRUE(p != nullptr);
}

TEST_F(heterogeneous_pool_test, object_belongs_to_the_pool)
{
    auto p = pool.alloc<int>();
    EXPECT_TRUE(pool.belongs(p));

    int i;
    EXPECT_FALSE(pool.belongs(&i));

}

TEST_F(heterogeneous_pool_test, allocates_same_object_once_freed)
{
    auto p1 = pool.alloc<int>();
    pool.dealloc(p1);
    auto p2 = pool.alloc<int>();
    EXPECT_EQ(p1, p2);
}

TEST_F(heterogeneous_pool_test, allocates_object_at_different_addr)
{
    int* p1 = pool.alloc<int>();
    int* p2 = pool.alloc<int>();

    EXPECT_NE(p1, p2);
}

TEST_F(heterogeneous_pool_test, returns_capacity)
{
    EXPECT_EQ(10, pool.capacity());
}

TEST_F(heterogeneous_pool_test, initial_size_is_zero)
{
    EXPECT_EQ(0, pool.size());
}

TEST_F(heterogeneous_pool_test, size_is_increased_as_a_result_of_object_allocation)
{
    pool.alloc<int>();
    EXPECT_EQ(1, pool.size());
}

TEST_F(heterogeneous_pool_test, exhausts_pool)
{
    haisu::heterogeneous_pool<1, int> pool;
    pool.alloc<int>();
    EXPECT_EQ(nullptr, pool.alloc<int>());
}

TEST_F(heterogeneous_pool_test, constructs_object)
{
    haisu::heterogeneous_pool<1, std::string> pool;
    auto s = pool.construct<std::string>("hello world");
    EXPECT_EQ("hello world", *s);
    pool.destroy(s);
}

TEST_F(heterogeneous_pool_test, size_is_decreased_once_the_object_gets_freed)
{
    auto p = pool.alloc<int>();
    pool.dealloc(p);

    EXPECT_EQ(0, pool.size());
}

TEST_F(heterogeneous_pool_test, deallocs_all_objects_without_calling_the_descructors)
{
    auto p = pool.alloc<int>();
    pool.dealloc_all();

    EXPECT_EQ(0, pool.size());
}

TEST_F(heterogeneous_pool_test, allocates_object_of_different_types)
{
    haisu::heterogeneous_pool<10, int, std::string> pool;
    auto p1 = pool.construct<int>();
    auto p2 = pool.construct<std::string>();

    EXPECT_EQ(2, pool.size());

    pool.destroy(p2);
}

