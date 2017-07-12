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

