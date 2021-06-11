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
#include "haisu/intrusive.h"
#include "haisu/duo.h"

template <typename T>
struct list_tests : public ::testing::Test
{
    using mylist = T;
    mylist list;
};

typedef ::testing::Types<
        haisu::list<int>,
        haisu::duo::list<int, 1>,
        haisu::duo::list<int, 4>,
        haisu::duo::list<int, 8>,
        haisu::duo::list<int, 16>,
        haisu::duo::list<int, 128>,
        haisu::duo::list<int, 1024>
    > TestTypes;
TYPED_TEST_CASE(list_tests, TestTypes);


TYPED_TEST(list_tests, list_is_empty_once_created)
{
    EXPECT_TRUE(this->list.empty());
}

TYPED_TEST(list_tests, list_is_not_empty)
{
    this->list.push_back(123);
    EXPECT_TRUE(!this->list.empty());
}

TYPED_TEST(list_tests, insert_node_in_empty_list)
{
    this->list.push_back(123);
    EXPECT_EQ(123, this->list.front()); 
}


TYPED_TEST(list_tests, back_points_to_newly_inserted_node)
{
    this->list.push_back(123);
    EXPECT_EQ(123, this->list.back()); 
}

TYPED_TEST(list_tests, inserts_several_nodes)
{
    this->list.push_back(123);
    this->list.push_back(456);
    this->list.push_back(789);

    EXPECT_EQ(123, this->list.front()); 
    EXPECT_EQ(789, this->list.back());
}

TYPED_TEST(list_tests, count_list_size)
{
    this->list.push_back(123);
    this->list.push_back(456);

    EXPECT_EQ(2, this->list.size());
}

TYPED_TEST(list_tests, empty_list_has_size_zero)
{
    EXPECT_EQ(0, this->list.size());
}

TYPED_TEST(list_tests, empties_list_by_popping_a_node_from_back)
{
    this->list.push_back(123);

    this->list.pop_back();
    EXPECT_EQ(0, this->list.size());
}

TYPED_TEST(list_tests, empties_list_by_poppsing_a_node_from_front)
{
    this->list.push_back(123);

    this->list.pop_front();
    EXPECT_EQ(0, this->list.size());
}

TYPED_TEST(list_tests, pops_node_from_front)
{
    this->list.push_back(123);
    this->list.push_back(456);
    this->list.pop_front();

    EXPECT_EQ(456, this->list.front());
}

TYPED_TEST(list_tests, pops_node_from_back)
{
    this->list.push_back(123);
    this->list.push_back(456);
    this->list.pop_back();

    EXPECT_EQ(123, this->list.back());
}

TYPED_TEST(list_tests, clears_list)
{
    this->list.push_back(123);
    this->list.clear();

    EXPECT_TRUE(this->list.empty());
}

TYPED_TEST(list_tests, concats_lists)
{
    this->list.push_back(123);

    TypeParam other;
    other.push_back(456);

    this->list.push_back(std::move(other));
    EXPECT_EQ(2, this->list.size());
}

TYPED_TEST(list_tests, concats_long_lists)
{
    this->list.push_back(12);
    this->list.push_back(34);
    ASSERT_EQ(2, this->list.size());

    TypeParam other;
    other.push_back(56);
    other.push_back(78);
    ASSERT_EQ(2, this->list.size());

    this->list.push_back(std::move(other));
    EXPECT_EQ(4, this->list.size());
}

TYPED_TEST(list_tests, initializes_list)
{
    TypeParam other;
    other.push_back(456);

    this->list.push_back(std::move(other));
    EXPECT_EQ(1, this->list.size());
}

TYPED_TEST(list_tests, appends_empty_list)
{
    TypeParam other;

    this->list.push_back(123);
    this->list.push_back(std::move(other));
    EXPECT_EQ(1, this->list.size());
}

TYPED_TEST(list_tests, creates_element_at_the_back)
{
    this->list.push_back(123);
    this->list.emplace_back() = 456;

    EXPECT_EQ(456, this->list.back()); 
}

TYPED_TEST(list_tests, creates_element_at_the_front)
{
    this->list.push_back(123);
    this->list.emplace_front() = 456;

    EXPECT_EQ(456, this->list.front()); 
}

/*
TYPED_TEST(list_tests, iterates_over_list)
{
    auto data = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    TypeParam list = data;
    std::vector<int> expected = data;
    std::vector<int> actual;

    for (auto i = list.begin(); i != list.end(); ++i) actual.push_back(*i);

    EXPECT_EQ(expected, actual);
}
*/
