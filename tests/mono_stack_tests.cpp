#include <gtest/gtest.h>
#include "haisu/mono.h"

struct mono_stack_test : ::testing::Test
{
	using int_stack = haisu::mono::stack<int>;

	
	int_stack stack;
};

TEST_F(mono_stack_test, creates_empty_stack)
{
	EXPECT_TRUE(stack.empty());
	EXPECT_EQ(0, stack.size());
}

TEST_F(mono_stack_test, stack_is_no_longer_empty)
{
	stack.push(1);

	EXPECT_FALSE(stack.empty());
}

TEST_F(mono_stack_test, maintains_stack_size)
{
	stack.push(1);
	stack.push(2);

	EXPECT_EQ(2, stack.size());
}

TEST_F(mono_stack_test, changes_size_when_poppin_an_element)
{
	stack.push(1);
	stack.pop();

	EXPECT_TRUE(stack.empty());
}

TEST_F(mono_stack_test, reads_top_of_stack)
{
	stack.push(1);

	EXPECT_EQ(1, stack.top());
}

TEST_F(mono_stack_test, uses_operator_to_append_stack)
{
	stack += 1;

	EXPECT_EQ(1, stack.top());
}

TEST_F(mono_stack_test, constructs_from_initializer_list)
{
	haisu::mono::stack<int> other = {1, 2};

	EXPECT_EQ(2, other.size());
	EXPECT_EQ(2, other.back());
	EXPECT_EQ(1, other.front());
}

TEST_F(mono_stack_test, appends_another_stack)
{
	haisu::mono::stack<int, 3> other = {1, 2, 3};
	stack += other;

	EXPECT_EQ(3, stack.size());
}

TEST_F(mono_stack_test, compares_equal_stacks)
{
	int_stack lhs = {1, 2, 3};
	int_stack rhs = {1, 2, 3};

	EXPECT_TRUE(lhs == rhs);
	EXPECT_FALSE(lhs != rhs);
}

TEST_F(mono_stack_test, compares_different_stacks_of_same_size)
{
	int_stack lhs = {1, 2, 3};
	int_stack rhs = {1, 2, 4};

	EXPECT_TRUE(lhs != rhs);
	EXPECT_FALSE(lhs == rhs);
}

TEST_F(mono_stack_test, compares_stacks_of_different_sizes)
{
	int_stack lhs = {1, 2, 3};
	int_stack rhs = {1, 2};

	EXPECT_TRUE(lhs != rhs);
	EXPECT_FALSE(lhs == rhs);
}

TEST_F(mono_stack_test, compares_equal_stacks_of_different_capacities)
{
	haisu::mono::stack<int, 3> lhs = {1, 2, 3};
	haisu::mono::stack<int, 1024> rhs = {1, 2, 3};

	EXPECT_TRUE(lhs == rhs);
	EXPECT_FALSE(lhs != rhs);
}

TEST_F(mono_stack_test, returns_stack_capacity)
{
	haisu::mono::stack<int, 3> stack;
	EXPECT_EQ(3, stack.capacity());
}

TEST_F(mono_stack_test, orders_stacks)
{
	EXPECT_FALSE(int_stack{1} < int_stack{1});
	EXPECT_TRUE(int_stack{1} < int_stack{2});
	EXPECT_FALSE(int_stack{2} < int_stack{1});

	EXPECT_TRUE((int_stack{1, 2} < int_stack{1, 2, 3}));
	EXPECT_FALSE((int_stack{1, 2, 3} < int_stack{1, 2}));
	EXPECT_FALSE((int_stack{1, 2} < int_stack{1, 2}));

	EXPECT_TRUE(int_stack() < int_stack{1});
	EXPECT_FALSE(int_stack() < int_stack());
}

TEST_F(mono_stack_test, iterates_over_stack)
{
	std::vector<int> expected = {1, 2, 3};
	std::vector<int> actual;

	stack = int_stack{1, 2, 3};

	for (auto i: stack)
	{
		actual.push_back(i);
	}

	EXPECT_EQ(expected, actual);
}