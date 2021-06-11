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
#include <memory>
#include "haisu/small_any.h"

struct small_any_test : ::testing::Test
{
    using small_any = haisu::small_any;
};

TEST_F(small_any_test, creates_any_object)
{
    small_any o(123);
    EXPECT_EQ(123, o.unsafe_cast<int>());
}

TEST_F(small_any_test, constructs_object)
{
    small_any o(std::make_shared<int>(123));
    EXPECT_EQ(123, *o.unsafe_cast<std::shared_ptr<int>>());
}

TEST_F(small_any_test, accesses_stored_object)
{
    small_any o(123);
    o.cast<int>() = 456;
    EXPECT_EQ(456, o.cast<int>());
}

TEST_F(small_any_test, copies_object)
{
    auto p = std::make_shared<int>(123);
    small_any o1(p);
    small_any o2(o1);

    EXPECT_EQ(123, *o1.cast<decltype(p)>());
    EXPECT_EQ(123, *o2.cast<decltype(p)>());
}

TEST_F(small_any_test, resets_object)
{
    small_any o(123);
    o.reset(456);

    EXPECT_EQ(456, o.cast<int>());
}

TEST_F(small_any_test, assignes_object)
{
    small_any lhs(123);
    small_any rhs;
    
    rhs = lhs;

    EXPECT_EQ(123, rhs.cast<int>());
}

TEST_F(small_any_test, moves_object)
{
    auto p = std::make_shared<int>(123);
    small_any o1(p);
    small_any o2(std::move(o1));

    EXPECT_EQ(123, *o2.cast<decltype(p)>());
    EXPECT_TRUE(nullptr == o1.cast<decltype(p)>().get());
}

TEST_F(small_any_test, move_assignes_object)
{
    auto p = std::make_shared<int>(123);
    small_any o1(p);
    small_any o2;
    
    o2 = std::move(o1);

    EXPECT_EQ(123, *o2.cast<decltype(p)>());
    EXPECT_TRUE(nullptr == o1.cast<decltype(p)>().get());
}

TEST_F(small_any_test, creates_empty_object)
{
    small_any o;
    EXPECT_TRUE(o.empty());
}

TEST_F(small_any_test, create_non_empty_object)
{
    small_any o(123);
    EXPECT_FALSE(o.empty());
}

TEST_F(small_any_test, resets_object_to_empty_state)
{
    small_any o(123);
    o.clear();
    EXPECT_TRUE(o.empty());
}

TEST_F(small_any_test, swaps_objects)
{
    small_any lhs(123);
    small_any rhs(456);

    lhs.swap(rhs);

    EXPECT_EQ(123, rhs.cast<int>());
    EXPECT_EQ(456, lhs.cast<int>());
}

