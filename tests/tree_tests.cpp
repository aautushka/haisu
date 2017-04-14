#include <gtest/gtest.h>
#include "haisu/tree.h"
#include "haisu/mono.h"

struct tree_test : ::testing::Test
{
	haisu::tree<int, int> tree;
};

TEST_F(tree_test, assignes_tree_root)
{
	tree[1] = 2;
	EXPECT_EQ(2, tree[1]);
}

TEST_F(tree_test, assignes_tree_branch)
{
	auto path = {1, 2, 3};
	tree[path] = 4;
	EXPECT_EQ(4, tree[path]);
}

TEST_F(tree_test, tracks_back_branch_nodes)
{
	auto p1 = {1, 2, 3};
	auto p2 = {1, 2};
	auto p3 = {1};

	tree[p1] = 4;
	tree[p2] = 5;
	tree[p3] = 6;

	EXPECT_EQ(4, tree[p1]);
	EXPECT_EQ(5, tree[p2]);
	EXPECT_EQ(6, tree[p3]);
}

TEST_F(tree_test, can_use_any_iteratorable_container_as_key)
{
	haisu::mono::stack<int> key1 = {1, 2, 3};
	std::vector<int> key2 = {1, 2, 3};

	tree[key1] = 4;
	EXPECT_EQ(tree[key2], tree[key1]);
}

TEST_F(tree_test, creates_empty_tree)
{
	EXPECT_TRUE(tree.empty());
}

TEST_F(tree_test, tree_is_no_longer_empty)
{
	tree[1] = 2;
	EXPECT_FALSE(tree.empty());
}

TEST_F(tree_test, recursively_iterates_over_tree)
{
	std::vector<std::pair<int, int>> actual; 
	std::vector<std::pair<int, int>> expected = {{1, 2}, {3, 4}, {5, 6}};

	using path = std::vector<int>;

	tree[path{1}] = 2;
	tree[path{1, 3}] = 4;
	tree[path{1, 3, 5}] = 6;

	tree.foreach([&](auto key, auto val){actual.push_back(std::make_pair(key, val));});
	EXPECT_EQ(expected, actual);
}
