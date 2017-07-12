/*
MIT License

Copyright (c) 2017 Anton Autushka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <gtest/gtest.h>
#include "haisu/tree.h"
#include "haisu/mono_stack.h"

struct tree_test : ::testing::Test
{
	haisu::tree<int, int> tree;
	using path = std::vector<int>;

	void replace_all(std::string& str, const std::string& what, const std::string& with)
	{
		size_t pos = str.find(what, 0);
		while (pos < str.size() && pos != std::string::npos)
		{
			str.replace(pos, what.size(), with);
			pos += with.size();
			pos = str.find(what, pos);
		}
	}

	std::string beautify(std::string str)
	{
		std::string ret(str);
		replace_all(ret, "\"", "");
		replace_all(ret, " ", "");
		return ret;
	}

	std::string to_json()
	{
		return beautify(haisu::to_json(tree));
	}
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

	tree[path{1}] = 2;
	tree[path{1, 3}] = 4;
	tree[path{1, 3, 5}] = 6;

	tree.foreach([&](auto key, auto val){actual.push_back(std::make_pair(key, val));});
	EXPECT_EQ(expected, actual);
}

TEST_F(tree_test, counts_existing_element)
{
	tree[1] = 2;

	EXPECT_EQ(1, tree.count(1));
}

TEST_F(tree_test, counts_existing_element_on_a_branch)
{
	tree[path{1, 2, 3}] = 4;

	EXPECT_EQ(1, tree.count(path{1, 2, 3}));
}

TEST_F(tree_test, counts_non_existant_elements)
{
	EXPECT_EQ(0, tree.count(1));
	EXPECT_EQ(0, tree.count(path{1, 2, 3}));
}

TEST_F(tree_test, erases_leaf)
{
	tree[1] = 2;
	tree.erase(1);

	EXPECT_EQ(0, tree.count(1));
}

TEST_F(tree_test, does_not_touch_parent_nodes_when_erasing_element)
{
	tree[path{1, 2, 3}] = 4;
	tree.erase(path{1, 2, 3});

	EXPECT_EQ(0, tree.count(path{1, 2, 3}));
	EXPECT_EQ(1, tree.count(path{1, 2}));
}

TEST_F(tree_test, removes_whole_branch)
{
	tree[path{1, 2, 3}] = 4;
	tree.erase(1);

	EXPECT_EQ(0, tree.count(path{1, 2, 3}));
	EXPECT_EQ(0, tree.count(path{1, 2}));
	EXPECT_EQ(0, tree.count(1));
}

TEST_F(tree_test, clears_whole_tree)
{
	tree[1] = 2;
	tree.clear();

	EXPECT_EQ(0, tree.count(1));
}

TEST_F(tree_test, count_number_of_tree_nodes)
{
	tree[path{1, 2, 3}] = 4;

	EXPECT_EQ(3, tree.size());
}

TEST_F(tree_test, serializes_empty_tree)
{
	EXPECT_EQ("{}", to_json());
}

TEST_F(tree_test, serializes_shallow_tree_to_json)
{
	tree[1] = 1;
	tree[2] = 2;

	EXPECT_EQ("{1:1,2:2}", to_json());			
}

TEST_F(tree_test, serializes_deep_tree_to_json)
{
	tree[1] = 1;
	tree[path{1, 2}] = 2;
	tree[path{1, 2, 3}] = 3;

	EXPECT_EQ("{1:{#:1,2:{#:2,3:3}}}", to_json());			
}
