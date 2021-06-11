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
