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

#include "haisu/zset.h"

struct zset_test: ::testing::Test
{
    haisu::zset z;
};

TEST_F(zset_test, newly_create_zmap_is_empty)
{
    EXPECT_TRUE(z.empty());
    EXPECT_EQ(0u, z.size());
}

TEST_F(zset_test, zset_is_no_longer_empty_when_adding_somethibg)
{
    z.insert("hello");
    EXPECT_FALSE(z.empty());
}

TEST_F(zset_test, zset_returns_number_of_element)
{
    z.insert("hello");
    EXPECT_EQ(1u, z.size());
}

TEST_F(zset_test, returns_inserted_string)
{
    z.insert("hello");
    EXPECT_STREQ("hello", z.at(0));
}

TEST_F(zset_test, inserts_multiple_strings)
{
    z.insert("hello");
    z.insert("world");

    EXPECT_STREQ("hello", z.at(0));
    EXPECT_STREQ("world", z.at(1));
}

TEST_F(zset_test, capacity_is_zero_before_any_insertion)
{
    EXPECT_EQ(0u, z.capacity());
}

TEST_F(zset_test, capacity_increased_after_insertion)
{
    z.insert("hello");
    EXPECT_LT(0u, z.capacity());
}

TEST_F(zset_test, clears_zset)
{
    z.insert("hello");
    z.clear();

    EXPECT_EQ(0u, z.size());
}

TEST_F(zset_test, finds_element)
{
    z.insert("hello");

    EXPECT_EQ(1u, z.count("hello"));
}

TEST_F(zset_test, fails_to_find_element)
{
    z.insert("hello");

    EXPECT_EQ(0u, z.count("world"));
}

TEST_F(zset_test, updates_size_when_erasing_element)
{
    z.insert("hello");
    z.erase("hello");

    EXPECT_EQ(0u, z.size());
}

TEST_F(zset_test, erases_element_from_head)
{
    z.insert("hello");
    z.insert("world");
    
    z.erase("hello");

    EXPECT_STREQ("world", z.at(0));
}

TEST_F(zset_test, erases_element_from_tail)
{
    z.insert("hello");
    z.insert("world");
    
    z.erase("world");
    z.insert("mundo");

    EXPECT_STREQ("mundo", z.at(1));
}

TEST_F(zset_test, erases_buffer_from_the_middle)
{
    z.insert("hello");
    z.insert("new");
    z.insert("world");
    
    z.erase("new");
    
    EXPECT_STREQ("hello", z.at(0));
    EXPECT_STREQ("world", z.at(1));
}

TEST_F(zset_test, maintains_sorted_order)
{
    z.insert("world");
    z.insert("hello");

    EXPECT_STREQ("hello", z.at(0));
    EXPECT_STREQ("world", z.at(1));
}

TEST_F(zset_test, lower_bound_finds_first_elemnent_which_is_greater_than_key)
{
    z.insert("hello");
    z.insert("world");

    EXPECT_STREQ("world", *z.lower_bound("new"));
}

TEST_F(zset_test, lower_bound_finds_elemnent_which_is_equal_to_key)
{
    z.insert("hello");

    EXPECT_STREQ("hello", *z.lower_bound("hello"));
}

TEST_F(zset_test, lower_bound_returns_valid_iterator)
{
    z.insert("hello");

    EXPECT_TRUE(z.lower_bound("hello") != z.end());
}

TEST_F(zset_test, lower_bound_cant_find_element_which_is_not_less_than_key)
{
    z.insert("hello");

    EXPECT_TRUE(z.lower_bound("zzz") == z.end());
}

TEST_F(zset_test, begin_points_to_first_element)
{
    z.insert("hello");

    EXPECT_STREQ("hello", *z.begin());
}

TEST_F(zset_test, begin_equals_end_in_empty_zset)
{
    EXPECT_TRUE(z.begin() == z.end());
}

TEST_F(zset_test, upper_bound_finds_first_elemnent_which_is_greater_than_key)
{
    z.insert("hello");
    z.insert("world");

    EXPECT_STREQ("world", *z.upper_bound("new"));
}

TEST_F(zset_test, upper_bound_returns_end_if_no_containing_element_greater_than_key)
{
    z.insert("hello");

    EXPECT_TRUE(z.upper_bound("hello") == z.end());
}

TEST_F(zset_test, increments_size_when_inserting)
{
    z.insert("hello");
    z.insert("world");
    
    EXPECT_EQ(2u, z.size());
}

TEST_F(zset_test, increments_size_when_inserting_in_revesrse_order)
{
    z.insert("world");
    z.insert("hello");
    
    EXPECT_EQ(2u, z.size());
}

TEST_F(zset_test, size_does_not_change_when_inserting_duplicate)
{
    z.insert("hello");
    z.insert("world");
    z.insert("world");

    EXPECT_EQ(2u, z.size());
}

TEST_F(zset_test, decrements_size_when_erasing)
{
    z.insert("hello");
    z.insert("world");
    z.erase("hello");

    EXPECT_EQ(1u, z.size());
}

TEST_F(zset_test, increments_iterator)
{
    z.insert("hello");
    z.insert("world");

    EXPECT_STREQ("world", *(++z.begin()));
}

TEST_F(zset_test, increments_iterator_beyond_boundaries)
{
    z.insert("hello");

    EXPECT_TRUE(++z.begin() == z.end());
}

TEST_F(zset_test, post_increment_iterator_returns_prev_item)
{
    z.insert("hello");
    z.insert("world");

    EXPECT_STREQ("hello", *(z.begin()++));
}

TEST_F(zset_test, post_increments_iterator)
{
    z.insert("hello");
    z.insert("world");

    haisu::zset::iterator i = z.begin();
    i++;

    EXPECT_STREQ("world", *i);
}

TEST_F(zset_test, first_iterator_in_equal_range_is_the_one_returned_by_lower_bound)
{
    z.insert("hello");
    z.insert("new");
    z.insert("world");

    auto p = z.equal_range("new");
    EXPECT_STREQ("new", *p.first);
}

TEST_F(zset_test, first_iterator_in_equal_range_is_the_one_returned_by_upper_bound)
{
    z.insert("hello");
    z.insert("world");

    auto p = z.equal_range("new");
    EXPECT_STREQ("world", *p.first);
}

TEST_F(zset_test, first_iterator_in_equal_range_is_the_one_returned_by_uppert_bound)
{
    z.insert("hello");
    z.insert("new");
    z.insert("world");

    auto p = z.equal_range("new");
    EXPECT_STREQ("world", *p.second);
}

TEST_F(zset_test, equal_range_fails_to_find_anything)
{
    z.insert("hello");

    auto p = z.equal_range("world");
    EXPECT_TRUE(p == std::make_pair(z.end(), z.end()));
}

TEST_F(zset_test, default_constructed_iterator_compares_to_end)
{
    haisu::zset::iterator i;
    EXPECT_TRUE(i == z.end());
}

TEST_F(zset_test, finds_specific_key)
{
    z.insert("hello");

    EXPECT_STREQ("hello", *z.find("hello"));
}

TEST_F(zset_test, fails_to_find_key)
{
    z.insert("hello");

    EXPECT_TRUE(z.end() == z.find("world"));
}

TEST_F(zset_test, constructs_from_initializer_list)
{
    haisu::zset z = {"hello", "world"};

    EXPECT_STREQ("hello", z.at(0));
    EXPECT_STREQ("world", z.at(1));
}

TEST_F(zset_test, sorts_data_when_constructing_from_initializer_list)
{
    haisu::zset z = {"world", "hello"};

    EXPECT_STREQ("hello", z.at(0));
    EXPECT_STREQ("world", z.at(1));
}

