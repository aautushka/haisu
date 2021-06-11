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
#include "haisu/mono_queue.h"

struct mono_queue_test : public ::testing::Test
{
    haisu::mono::queue<int, 4> q;
};

TEST_F(mono_queue_test, queue_is_empty_once_created)
{
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(0, q.size());
    EXPECT_FALSE(q.full());
}

TEST_F(mono_queue_test, queue_is_no_longer_empty)
{
    q.push_back(123);
    EXPECT_FALSE(q.empty());
}

TEST_F(mono_queue_test, accesses_element_just_added_to_the_tail)
{
    q.push_back(123);
    EXPECT_EQ(123, q.front());
    EXPECT_EQ(123, q.back());
}

TEST_F(mono_queue_test, accesses_element_just_added_to_the_head)
{
    q.push_front(123);
    EXPECT_EQ(123, q.front());
    EXPECT_EQ(123, q.back());
}

TEST_F(mono_queue_test, accesses_head_and_tail_separately)
{
    q.push_back(123);
    q.push_back(456);

    EXPECT_EQ(123, q.front());
    EXPECT_EQ(456, q.back());
}

TEST_F(mono_queue_test, pushes_elements_to_the_head)
{
    q.push_front(123);
    q.push_front(456);

    EXPECT_EQ(456, q.front());
    EXPECT_EQ(123, q.back());
}

TEST_F(mono_queue_test, queue_capacity_matches_the_template_argument)
{
    EXPECT_EQ(4, q.capacity());
}

TEST_F(mono_queue_test, queue_is_not_full_until_consumes_all_capacity)
{
    q.push_back(1);
    q.push_back(2);
    q.push_back(3);

    EXPECT_FALSE(q.full());
}

TEST_F(mono_queue_test, queue_is_full)
{
    q.push_back(1);
    q.push_back(2);
    q.push_back(3);
    q.push_back(4);

    EXPECT_EQ(4, q.size());
    EXPECT_TRUE(q.full());
}

TEST_F(mono_queue_test, reaches_full_capacity_by_growing_queue_from_its_head)
{
    q.push_front(1);
    q.push_front(2);
    q.push_front(3);
    q.push_front(4);

    EXPECT_EQ(4, q.size());
    EXPECT_TRUE(q.full());
}


TEST_F(mono_queue_test, clears_queue)
{
    q.push_back(1);
    q.clear();

    EXPECT_TRUE(q.empty());
}

TEST_F(mono_queue_test, push_back_changes_queue_size)
{
    q.push_back(1);

    EXPECT_EQ(1, q.size());
}

TEST_F(mono_queue_test, push_front_changes_queue_size)
{
    q.push_front(1);

    EXPECT_EQ(1, q.size());
}

TEST_F(mono_queue_test, pop_front_changes_queue_size)
{
    q.push_front(1);
    q.pop_front();

    EXPECT_EQ(0, q.size());
}

TEST_F(mono_queue_test, pop_back_changes_queue_size)
{
    q.push_front(1);
    q.pop_back();

    EXPECT_EQ(0, q.size());
}

TEST_F(mono_queue_test, pop_back_returns_element_being_removed)
{
    q.push_back(1);
    q.push_back(2);

    EXPECT_EQ(2, q.pop_back());
}

TEST_F(mono_queue_test, pop_front_returns_element_being_removed)
{
    q.push_back(1);
    q.push_back(2);

    EXPECT_EQ(1, q.pop_front());
}

TEST_F(mono_queue_test, queue_at_full_capacity_it_not_empty)
{
    q.push_back(1);
    q.push_back(2);
    q.push_back(3);
    q.push_back(4);

    EXPECT_FALSE(q.empty());
}

TEST_F(mono_queue_test, iterates_over_queue)
{
    q.push_back(1);
    q.push_back(2);
    q.push_back(3);
    q.push_back(4);

    std::vector<int> expected = {1, 2, 3, 4};
    std::vector<int> actual;

    for (int i : q ) actual.push_back(i);
    EXPECT_EQ(expected, actual);

}

TEST_F(mono_queue_test, creates_element_at_the_back)
{
    q.push_back(123);
    q.emplace_back() = 456;

    EXPECT_EQ(456, q.back()); 
}

TEST_F(mono_queue_test, creates_element_at_the_front)
{
    q.push_back(123);
    q.emplace_front() = 456;

    EXPECT_EQ(456, q.front()); 
}
