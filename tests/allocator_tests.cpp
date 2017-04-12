#include <gtest/gtest.h>
#include "haisu/memory.h"

struct allocator_test : ::testing::Test
{
	using mem_t = haisu::mem::podbump;
	using allocator_t = haisu::mem::allocator<int, mem_t>;

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
