#include <gtest/gtest.h>

#include <vector>
#include "haisu/algo.h"

using haisu::algo::binary_search;
using haisu::algo::npos;

struct binary_search_test: ::testing::Test
{
};

TEST_F(binary_search_test, finds_all_number_in_vector)
{
	std::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

	for (int i : v)
	{
		EXPECT_EQ(i, binary_search(v, v[i]));
	}
}

TEST_F(binary_search_test, fails_to_find_enything_in_empty_array)
{
	std::vector<int> v;;

	EXPECT_EQ(npos, binary_search(v, 1));
}

TEST_F(binary_search_test, fails_to_find_enything_in_single_item_array)
{
	std::vector<int> v = {1};

	EXPECT_EQ(0, binary_search(v, 1));
}

TEST_F(binary_search_test, finds_match_in_single_item_array)
{
	std::vector<int> v = {1};

	EXPECT_EQ(0, binary_search(v, 1));
}

TEST_F(binary_search_test, fails_to_find_matches_in_array)
{
	std::vector<int> v = {2, 4, 6, 8, 10};

	EXPECT_EQ(npos, binary_search(v, 1));
	EXPECT_EQ(npos, binary_search(v, 3));
	EXPECT_EQ(npos, binary_search(v, 5));
	EXPECT_EQ(npos, binary_search(v, 7));
	EXPECT_EQ(npos, binary_search(v, 9));
	EXPECT_EQ(npos, binary_search(v, 11));
}

