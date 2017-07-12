#include <gtest/gtest.h>
#include "haisu/object_pool.h"

struct object_pool_test : ::testing::Test
{
	haisu::object_pool<int, 10> pool;
};

TEST_F(object_pool_test, allocates_from_pool)
{
	int* p = pool.alloc();
	EXPECT_TRUE(p != nullptr);
}

TEST_F(object_pool_test, object_belongs_to_the_pool)
{
	auto p = pool.alloc();
	EXPECT_TRUE(pool.belongs(p));

	int i;
	EXPECT_FALSE(pool.belongs(&i));

}

TEST_F(object_pool_test, allocates_same_object_once_freed)
{
	auto p1 = pool.alloc();
	pool.dealloc(p1);
	auto p2 = pool.alloc();
	EXPECT_EQ(p1, p2);
}

TEST_F(object_pool_test, allocates_object_at_different_addr)
{
	int* p1 = pool.alloc();
	int* p2 = pool.alloc();

	EXPECT_NE(p1, p2);
}

TEST_F(object_pool_test, returns_capacity)
{
	EXPECT_EQ(10, pool.capacity());
}

TEST_F(object_pool_test, initial_size_is_zero)
{
	EXPECT_EQ(0, pool.size());
}

TEST_F(object_pool_test, size_is_increased_as_a_result_of_object_allocation)
{
	pool.alloc();
	EXPECT_EQ(1, pool.size());
}

TEST_F(object_pool_test, exhausts_pool)
{
	haisu::object_pool<int, 1> pool;
	pool.alloc();
	EXPECT_EQ(nullptr, pool.alloc());
}

TEST_F(object_pool_test, constructs_object)
{
	haisu::object_pool<std::string, 1> pool;
	auto s = pool.construct("hello world");
	EXPECT_EQ("hello world", *s);
}

TEST_F(object_pool_test, size_is_decreased_once_the_object_gets_freed)
{
	auto p = pool.alloc();
    pool.dealloc(p);

	EXPECT_EQ(0, pool.size());
}

TEST_F(object_pool_test, deallocs_all_objects_without_calling_the_descructors)
{
    auto p = pool.alloc();
    pool.dealloc_all();

    EXPECT_EQ(0, pool.size());
}

TEST_F(object_pool_test, destroys_all_object_and_calls_the_respective_desctuctors)
{
    haisu::object_pool<std::unique_ptr<std::string>, 1> pool;
    auto s = pool.construct(new std::string("hello world"));
    pool.destroy_all();

    EXPECT_EQ(0, pool.size());
}

