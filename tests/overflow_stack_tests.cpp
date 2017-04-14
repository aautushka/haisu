#include <gtest/gtest.h>
#include "haisu/mono.h"

struct overflow_stack_test : ::testing::Test
{
	using int_stack = haisu::mono::overflow_stack<int>;

	template <int N>
	using overflow_stack = haisu::mono::overflow_stack<int, N>;
	
	int_stack stack;
};

TEST_F(overflow_stack_test, creates_empty_stack)
{
	EXPECT_TRUE(stack.empty());
	EXPECT_EQ(0, stack.size());
}

TEST_F(overflow_stack_test, stack_is_no_longer_empty)
{
	stack.push(1);

	EXPECT_FALSE(stack.empty());
}

TEST_F(overflow_stack_test, maintains_stack_size)
{
	stack.push(1);
	stack.push(2);

	EXPECT_EQ(2, stack.size());
}

TEST_F(overflow_stack_test, changes_size_when_poppin_an_element)
{
	stack.push(1);
	stack.pop();

	EXPECT_TRUE(stack.empty());
}

TEST_F(overflow_stack_test, reads_top_of_stack)
{
	stack.push(1);

	EXPECT_EQ(1, stack.top());
}

TEST_F(overflow_stack_test, uses_operator_to_append_stack)
{
	stack += 1;

	EXPECT_EQ(1, stack.top());
}

TEST_F(overflow_stack_test, constructs_from_initializer_list)
{
	haisu::mono::overflow_stack<int> other = {1, 2};

	EXPECT_EQ(2, other.size());
	EXPECT_EQ(2, other.back());
	EXPECT_EQ(1, other.front());
}

TEST_F(overflow_stack_test, appends_another_stack)
{
	haisu::mono::overflow_stack<int, 3> other = {1, 2, 3};
	stack += other;

	EXPECT_EQ(3, stack.size());
}

TEST_F(overflow_stack_test, compares_equal_stacks)
{
	int_stack lhs = {1, 2, 3};
	int_stack rhs = {1, 2, 3};

	EXPECT_TRUE(lhs == rhs);
	EXPECT_FALSE(lhs != rhs);
}

TEST_F(overflow_stack_test, compares_different_stacks_of_same_size)
{
	int_stack lhs = {1, 2, 3};
	int_stack rhs = {1, 2, 4};

	EXPECT_TRUE(lhs != rhs);
	EXPECT_FALSE(lhs == rhs);
}

TEST_F(overflow_stack_test, compares_stacks_of_different_sizes)
{
	int_stack lhs = {1, 2, 3};
	int_stack rhs = {1, 2};

	EXPECT_TRUE(lhs != rhs);
	EXPECT_FALSE(lhs == rhs);
}

TEST_F(overflow_stack_test, compares_equal_stacks_of_different_capacities)
{
	haisu::mono::overflow_stack<int, 3> lhs = {1, 2, 3};
	haisu::mono::overflow_stack<int, 1024> rhs = {1, 2, 3};

	EXPECT_TRUE(lhs == rhs);
	EXPECT_FALSE(lhs != rhs);
}

TEST_F(overflow_stack_test, returns_stack_capacity)
{
	haisu::mono::overflow_stack<int, 3> stack;
	EXPECT_EQ(3, stack.capacity());
}

TEST_F(overflow_stack_test, orders_stacks)
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

TEST_F(overflow_stack_test, iterates_over_stack)
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

TEST_F(overflow_stack_test, can_push_items_beyond_capacity)
{
	overflow_stack<1> stack;
	stack.push(1);
	stack.push(2);
	stack.push(3);

	EXPECT_EQ(3, stack.size());
}

TEST_F(overflow_stack_test, bring_stack_back_to_capacity)
{
	overflow_stack<1> stack;
	stack.push(1);
	stack.push(2);
	stack.push(3);
	stack.pop();
	stack.pop();

	EXPECT_EQ(1, stack.size());
	EXPECT_EQ(1, stack.top());
}

TEST_F(overflow_stack_test, ignores_overlown_data_when_comparing_stacks)
{
	overflow_stack<1> lhs = {1};
	overflow_stack<1> rhs = {1, 2, 3};

	EXPECT_TRUE(lhs == rhs);
	EXPECT_FALSE(lhs != rhs);
	EXPECT_FALSE(lhs < rhs);
}

TEST_F(overflow_stack_test, switches_to_overflow_state_while_appending_other_stack)
{
	overflow_stack<1> lhs = {1};
	overflow_stack<1> rhs = {1, 2, 3};

	lhs += rhs;
	EXPECT_EQ(4, lhs.size());
	EXPECT_TRUE(lhs.overflow());
}