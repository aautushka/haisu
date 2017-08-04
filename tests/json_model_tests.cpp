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

#include "haisu/json_model.h"

struct json_model_test : ::testing::Test
{
    haisu::json::model model;
};

TEST_F(json_model_test, parses_json_having_one_pair)
{
    model.parse("{'a':'b'}");
    EXPECT_TRUE(model.has_key("a"));
}

TEST_F(json_model_test, key_not_present)
{
    model.parse("{'a':'b'}");
    EXPECT_FALSE(model.has_key("b"));
}

TEST_F(json_model_test, parses_two_keys)
{
    model.parse("{'a':'b', 'c' : 'd'}");
    EXPECT_TRUE(model.has_key("a"));
    EXPECT_TRUE(model.has_key("c"));
}

TEST_F(json_model_test, parses_three_keys)
{
    model.parse("{'a':'b', 'c' : 'd', 'e' : 'f'}");
    EXPECT_TRUE(model.has_key("a"));
    EXPECT_TRUE(model.has_key("c"));
    EXPECT_TRUE(model.has_key("e"));
}

TEST_F(json_model_test, gets_child)
{
    model.parse("{'a':{'b':'c'}}");
    EXPECT_TRUE(model.child("a").has_key("b"));
}

TEST_F(json_model_test, counts_children)
{
    model.parse("{'a':{'b':'c'}, 'd':{'e':'f'}}");
    EXPECT_EQ(2, model.root().count());
    EXPECT_EQ(1, model.child("a").count());
    EXPECT_EQ(0, model.child("a").child("b").count());

    EXPECT_EQ(1, model.child("d").count());
}

TEST_F(json_model_test, recognizes_array)
{
    model.parse("[]");
    EXPECT_TRUE(model.root().is_array());
}

TEST_F(json_model_test, counts_array_items)
{
    model.parse("['a','b','c']");
    EXPECT_EQ(3, model.root().count_array());
}
