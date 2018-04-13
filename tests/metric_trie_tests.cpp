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
#include "haisu/metric.h"

struct metric_trie_test : ::testing::Test
{
    using trie_t = haisu::metric::trie<int, int>;
    trie_t trie;
};

TEST_F(metric_trie_test, adds_node)
{
    trie.down(11) = 123;
    EXPECT_EQ(123, trie.get());
}

TEST_F(metric_trie_test, adds_second_node)
{
    trie.down(11) = 123;
    trie.down(11) = 456;
    
    EXPECT_EQ(456, trie.get());
}

TEST_F(metric_trie_test, goes_one_node_up)
{
    trie.down(11) = 123;
    trie.down(22) = 456;
    
    EXPECT_EQ(456, trie.up());
    EXPECT_EQ(123, trie.get());
}

TEST_F(metric_trie_test, builds_stack)
{
    trie.down(1) = 1;
    trie.down(2) = 2;
    trie.down(3) = 3;
    
    EXPECT_EQ(1, trie.at({1}));
    EXPECT_EQ(2, trie.at({1, 2}));
    EXPECT_EQ(3, trie.at({1, 2, 3}));
}

TEST_F(metric_trie_test, never_clears_data)
{
    trie.down(1) = 1;
    trie.down(2) = 2;
    trie.down(3) = 3;

    trie.up();
    trie.up();
    trie.up();
    
    EXPECT_EQ(1, trie.at({1}));
    EXPECT_EQ(2, trie.at({1, 2}));
    EXPECT_EQ(3, trie.at({1, 2, 3}));
}

TEST_F(metric_trie_test, adds_sibling_node)
{
    trie.down(1) = 1;

    trie.down(2) = 2;
    trie.up();

    trie.down(3) = 3;
    trie.up();

    trie.down(4) = 4;
    trie.up();

    EXPECT_EQ(2, trie.at({1, 2}));
    EXPECT_EQ(3, trie.at({1, 3}));
    EXPECT_EQ(4, trie.at({1, 4}));
}

TEST_F(metric_trie_test, drills_down_the_stack)
{
    trie.create({1, 2, 3, 4}) = 4;
    EXPECT_EQ(4, trie.at({1, 2, 3, 4}));
}

TEST_F(metric_trie_test, check_node_existence)
{
    EXPECT_FALSE(trie.has({1}));
    EXPECT_FALSE(trie.has({1, 2}));

    trie.down(1);

    EXPECT_TRUE(trie.has({1}));
    EXPECT_FALSE(trie.has({1, 2}));

    trie.down(2);
    EXPECT_TRUE(trie.has({1, 2}));

    trie.up();
    EXPECT_TRUE(trie.has({1}));
    EXPECT_TRUE(trie.has({1, 2}));

    trie.down(3);
    EXPECT_TRUE(trie.has({1}));
    EXPECT_TRUE(trie.has({1, 2}));
    EXPECT_TRUE(trie.has({1, 3}));
}

TEST_F(metric_trie_test, timer_integration_test)
{
    haisu::metric::trie<int, haisu::metric::timer> trie;

    trie.down(1).start();
    trie.down(2).start();
    trie.up();
    trie.up();

    EXPECT_TRUE(trie.has({1}));
    EXPECT_TRUE(trie.has({1, 2}));
}
