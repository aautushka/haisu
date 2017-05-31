#include <gtest/gtest.h>
#include "haisu/json.h"

struct json_bitstack_test : ::testing::Test
{
	template <int N>
	using bitstack = haisu::json::bitstack<N>;
	bitstack<1024> bits;
};

TEST_F(json_bitstack_test, pushes_true)
{
	bits.push<1>();
	EXPECT_TRUE(bits.top());
}

TEST_F(json_bitstack_test, pushes_false)
{
	bits.push<0>();
	EXPECT_FALSE(bits.top());
}

TEST_F(json_bitstack_test, pops_item)
{
	bits.push<1>();
	bits.push<0>();
	bits.pop();

	EXPECT_TRUE(bits.top());
}

TEST_F(json_bitstack_test, rewrites_false)
{
	bits.push<0>();
	bits.pop();
	bits.push<1>();
	EXPECT_TRUE(bits.top());
}

TEST_F(json_bitstack_test, rewrites_true)
{
	bits.push<1>();
	bits.pop();
	bits.push<0>();
	EXPECT_FALSE(bits.top());
}

TEST_F(json_bitstack_test, pops_several_items)
{
	bits.push<1>();
	bits.push<0>();
	bits.push<1>();
	bits.push<0>();

	EXPECT_EQ(false, bits.top());
	bits.pop();

	EXPECT_EQ(true, bits.top());
	bits.pop();

	EXPECT_EQ(false, bits.top());
	bits.pop();

	EXPECT_EQ(true, bits.top());
	bits.pop();
}

TEST_F(json_bitstack_test, pops_many_items)
{
	int loop = bits.capacity();
	for (int i = 0; i < loop; ++i)
	{
		bits.push(i % 2);
	}

	for (int i = loop; i > 0; --i)
	{
		EXPECT_EQ((i - 1) % 2, bits.top());
		bits.pop();
	}
}

TEST_F(json_bitstack_test, bitstack_63_specialization)
{
	bitstack<64> bits;
	int loop = bits.capacity();
	for (int i = 0; i < loop; ++i)
	{
		bits.push(i % 2);
	}

	for (int i = loop; i > 0; --i)
	{
		EXPECT_EQ((i - 1) % 2, bits.top());
		bits.pop();
	}
}

