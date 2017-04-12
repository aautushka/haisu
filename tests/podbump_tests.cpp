#include <gtest/gtest.h>
#include "haisu/memory.h"

struct podbump_test : ::testing::Test
{
	haisu::mem::podbump pod;
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
	haisu::mem::podbump pod(sizeof(int));
	int* p = pod.alloc<int>();
	ASSERT_TRUE(p != nullptr);

	EXPECT_EQ(nullptr, pod.alloc<int>());
}

TEST_F(podbump_test, recycles_memory)
{
	haisu::mem::podbump pod(sizeof(int));
	int* p = pod.alloc<int>();
	pod.free(p);

	EXPECT_NE(nullptr, pod.alloc<int>());
}
