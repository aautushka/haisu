
#include <gtest/gtest.h>
#include "haisu/memory.h"

struct refbump_test : ::testing::Test
{
	haisu::refbump memory;
};

TEST_F(refbump_test, allocates_memory)
{
	int* p = memory.alloc<int>();
	EXPECT_TRUE(p != nullptr);
}

TEST_F(refbump_test, new_allocation_goes_to_different_address)
{
	int* p1 = memory.alloc<int>();
	int* p2 = memory.alloc<int>();

	EXPECT_TRUE(p1 != p2);
}

TEST_F(refbump_test, subsequent_allocation_do_not_interfere_with_one_another)
{
	int* p1 = memory.alloc<int>();
	int* p2 = memory.alloc<int>();

	*p1 = 0xdeadbeef;
	*p2 = 0xbaddcafe;

	EXPECT_EQ(0xdeadbeef, *p1);
	EXPECT_EQ(0xbaddcafe, *p2);
}

TEST_F(refbump_test, automatically_adds_more_memory_if_needed)
{
	for (int i = 0; i < 100000; ++i)
	{
		void* p = memory.alloc(32);
		EXPECT_TRUE(nullptr != p);
	}
}

TEST_F(refbump_test, allocates_huge_buffer_all_at_once)
{
	void* p = memory.alloc(100 * 1024 * 1024);
	EXPECT_TRUE(nullptr != p);
}

TEST_F(refbump_test, allocates_memory_of_any_imaginable_size)
{
	for (int i = 1; i < 128 * 1024 * 1024; i *= 2)
	{
		haisu::refbump memory;
		void* p = memory.alloc(i);
		EXPECT_TRUE(nullptr != p);
	}
}

TEST_F(refbump_test, allocates_one_arena)
{
	int* p = memory.alloc<int>();

	EXPECT_EQ(1, memory.arena_count());
}

TEST_F(refbump_test, allocates_two_arenas)
{
	void* p1 = memory.alloc<int>();
	void* p2 = memory.alloc(10 * 1024 * 1024); // this guarantees we have an additional huge arena

	EXPECT_EQ(2, memory.arena_count());
}

