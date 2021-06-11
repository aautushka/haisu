/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
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

