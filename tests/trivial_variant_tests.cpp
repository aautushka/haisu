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
#include <experimental/string_view>
#include "haisu/trivial_variant.h"

struct trivial_variant_test : ::testing::Test
{

};

TEST_F(trivial_variant_test, holds_first_template_type_when_default_constructed)
{
    haisu::trivial_variant<int, char> v;
    EXPECT_EQ(0, v.index());
    EXPECT_NE(nullptr, v.get_if<int>());
}

TEST_F(trivial_variant_test, gets_null_when_trying_to_access_not_selected_type)
{
    haisu::trivial_variant<int, char> v;
    EXPECT_EQ(nullptr, v.get_if<char>());
}

TEST_F(trivial_variant_test, selects_another_type)
{
    haisu::trivial_variant<int, char> v;
    v.select<char>();

    EXPECT_EQ(1, v.index());
    EXPECT_NE(nullptr, v.get_if<char>());
}

TEST_F(trivial_variant_test, assigns_another_type)
{
    haisu::trivial_variant<int, char> v;
    v.assign('a');

    EXPECT_EQ('a', v.get<char>());
}

TEST_F(trivial_variant_test, asssignes_another_value_with_assignment_operator)
{
    haisu::trivial_variant<int, char> v;
    v = 'a';

    EXPECT_EQ('a', v.get<char>());
}

TEST_F(trivial_variant_test, assignes_string_view)
{
    haisu::trivial_variant<int, std::experimental::string_view> v;
    v = std::experimental::string_view("hello world");

    EXPECT_EQ("hello world", v.get<std::experimental::string_view>());
}
