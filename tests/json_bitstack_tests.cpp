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

