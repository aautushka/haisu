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
