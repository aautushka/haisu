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
#include "haisu/mono_list.h"

struct mono_list_tests : public ::testing::Test
{
    haisu::mono::list<int, 16> list;
};

TEST_F(mono_list_tests, list_is_empty)
{
    EXPECT_TRUE(list.empty());
}

TEST_F(mono_list_tests, list_not_empty)
{
    list.push_back(123);
    EXPECT_FALSE(list.empty());
}

TEST_F(mono_list_tests, list_not_full)
{
    EXPECT_FALSE(list.full());
}

TEST_F(mono_list_tests, list_is_full)
{
    haisu::mono::list<int, 3> list;
    list.push_back(123);
    list.push_back(123);
    list.push_back(123);
    EXPECT_TRUE(list.full());
}

TEST_F(mono_list_tests, reads_front)
{
    list.push_back(123);
    list.push_back(456);
    EXPECT_EQ(123, list.front());
}
        
TEST_F(mono_list_tests, reads_back)
{
    list.push_back(123);
    list.push_back(456);
    EXPECT_EQ(456, list.back());
}

TEST_F(mono_list_tests, front_and_back_are_same_in_a_single_element_list)
{
    list.push_back(123);
    EXPECT_EQ(list.front(), list.back());
}

TEST_F(mono_list_tests, pushes_items_to_front)
{
    list.push_front(123);
    list.push_front(456);
    EXPECT_EQ(456, list.front());
    EXPECT_EQ(123, list.back());
}

TEST_F(mono_list_tests, counts_number_of_list_entries)
{
    EXPECT_EQ(0, list.size());

    list.push_back(123);
    EXPECT_EQ(1, list.size());

    list.push_back(123);
    EXPECT_EQ(2, list.size());

    list.push_back(123);
    EXPECT_EQ(3, list.size());
}

TEST_F(mono_list_tests, fills_list_to_full_capacity)
{
    haisu::mono::list<int, 3> list;
    list.push_back(123);
    list.push_back(123);
    list.push_back(123);

    EXPECT_EQ(3, list.size());
}

TEST_F(mono_list_tests, creates_from_initializer_list)
{
    list = {1, 3};

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(1, list.front());
    EXPECT_EQ(3, list.back());
}

TEST_F(mono_list_tests, clears_list)
{
    list = {1, 2, 3};
    list.clear();

    EXPECT_EQ(0, list.size());
}

TEST_F(mono_list_tests, reuses_freed_nodes)
{
    haisu::mono::list<int, 3> list;
    list = {1, 2, 3};

    list.clear();

    list = {4, 5, 6};
    EXPECT_EQ(3, list.size());
}

TEST_F(mono_list_tests, pops_item_from_back)
{
    list = {1, 2, 3};

    EXPECT_EQ(3, list.pop_back());
    EXPECT_EQ(2, list.size());
}

TEST_F(mono_list_tests, clears_list_by_popping_back)
{
    list = {1};
    list.pop_back();
    EXPECT_TRUE(list.empty());
}

TEST_F(mono_list_tests, reuses_popped_item)
{
    haisu::mono::list<int, 1> list;
    list = {1};
    list.pop_back();
    list = {2};
    EXPECT_FALSE(list.empty());
}

TEST_F(mono_list_tests, pops_item_from_front)
{
    list = {1, 2, 3};

    EXPECT_EQ(1, list.pop_front());
    EXPECT_EQ(2, list.size());
}

TEST_F(mono_list_tests, clears_list_by_popping_front)
{
    list = {1};
    list.pop_front();
    EXPECT_TRUE(list.empty());
}

TEST_F(mono_list_tests, reuses_item_from_front)
{
    haisu::mono::list<int, 1> list;
    list = {1};
    list.pop_front();
    list = {2};
    EXPECT_FALSE(list.empty());
}

TEST_F(mono_list_tests, fill_the_list_up_to_addressing_boundary)
{
    haisu::mono::list<int, 254> list;
    for (int i = 0; i < 254; ++i)
    {
        list.push_back(i);
    }

    EXPECT_EQ(254, list.size());
}

TEST_F(mono_list_tests, creates_list_of_objects)
{
    haisu::mono::list<std::string, 1> list;
    list.push_back(std::string(1024, 'x'));

    EXPECT_TRUE(std::string(1024, 'x') == list.front());
}
