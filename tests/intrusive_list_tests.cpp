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
#include "haisu/intrusive.h"

struct intrusive_list_tests : public ::testing::Test
{
    using mylist = haisu::intrusive_list<int>;
    mylist list;
};

TEST_F(intrusive_list_tests, list_is_empty_once_created)
{
    EXPECT_TRUE(list.empty());
}

TEST_F(intrusive_list_tests, list_is_not_empty)
{
    mylist::node node(123);    
    list.push_back(node);
    EXPECT_TRUE(!list.empty());
}

TEST_F(intrusive_list_tests, insert_node_in_empty_list)
{
    mylist::node node(123);    
    list.push_back(node);
    EXPECT_EQ(123, list.front().data); 
}


TEST_F(intrusive_list_tests, back_points_to_newly_inserted_node)
{
    mylist::node node(123);    
    list.push_back(node);
    EXPECT_EQ(123, list.back().data); 
}

TEST_F(intrusive_list_tests, inserts_several_nodes)
{
    mylist::node n1(123);    
    mylist::node n2(456);
    mylist::node n3(789);

    list.push_back(n1);
    list.push_back(n2);
    list.push_back(n3);
    EXPECT_EQ(123, list.front().data); 
    EXPECT_EQ(789, list.back().data);
}

TEST_F(intrusive_list_tests, count_list_size)
{
    mylist::node n1(123);    
    mylist::node n2(456);

    list.push_back(n1);
    list.push_back(n2);

    EXPECT_EQ(2, list.size());
}

TEST_F(intrusive_list_tests, empty_list_has_size_zero)
{
    EXPECT_EQ(0, list.size());
}

TEST_F(intrusive_list_tests, empties_list_by_popping_a_node_from_back)
{
    mylist::node n(123);
    list.push_back(n);

    list.pop_back();
    EXPECT_EQ(0, list.size());
}

TEST_F(intrusive_list_tests, empties_list_by_poppsing_a_node_from_front)
{
    mylist::node n(123);
    list.push_back(n);

    list.pop_front();
    EXPECT_EQ(0, list.size());
}

TEST_F(intrusive_list_tests, pops_node_from_front)
{
    mylist::node n1(123);    
    mylist::node n2(456);

    list.push_back(n1);
    list.push_back(n2);
    list.pop_front();

    EXPECT_EQ(456, list.front());
}

TEST_F(intrusive_list_tests, pops_node_from_back)
{
    mylist::node n1(123);    
    mylist::node n2(456);

    list.push_back(n1);
    list.push_back(n2);
    list.pop_back();

    EXPECT_EQ(123, list.back());
}

TEST_F(intrusive_list_tests, clears_list)
{
    mylist::node n(123);
    list.push_back(n);
    list.clear();

    EXPECT_TRUE(list.empty());
}

TEST_F(intrusive_list_tests, clears_list_by_erasing_its_single_node)
{
    mylist::node n(123);
    list.push_back(n);
    list.erase(n);

    EXPECT_TRUE(list.empty());
}

TEST_F(intrusive_list_tests, erases_node_from_list_front)
{
    mylist::node n1(123);
    mylist::node n2(456);

    list.push_back(n1);
    list.push_back(n2);

    list.erase(n1);

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(456, list.front());
}

TEST_F(intrusive_list_tests, erases_node_from_list_back)
{
    mylist::node n1(123);
    mylist::node n2(456);

    list.push_back(n1);
    list.push_back(n2);

    list.erase(n2);

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(123, list.front());
}

TEST_F(intrusive_list_tests, erases_node_from_the_middle)
{
    mylist::node n1(123);
    mylist::node n2(456);
    mylist::node n3(789);

    list.push_back(n1);
    list.push_back(n2);
    list.push_back(n3);

    list.erase(n2);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(123, list.front());
    EXPECT_EQ(789, list.back());
}
