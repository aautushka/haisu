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
#include <vector>
#include <thread>
#include "haisu/concurrency.h"

struct synchronized_test : ::testing::Test
{
};

TEST_F(synchronized_test, synchronizes_access_to_an_object)
{
    haisu::synchronized<std::vector<int>> v;
    v->push_back(123);
    EXPECT_EQ(123, v->back());
}

TEST_F(synchronized_test, locks_mutex_multiple_times_within_the_same_expression)
{
    haisu::synchronized<std::vector<int>> v;
    v->push_back(1);
    v->push_back(2);

    EXPECT_EQ(3, v->front() + v->back());
}

TEST_F(synchronized_test, stress_test)
{
    haisu::synchronized<std::vector<int>> v;
    std::thread t1([&](){for (int i = 0; i < 1000000; ++i) v->push_back(i);}); 
    std::thread t2([&](){for (int i = 0; i < 1000000; ++i) v->push_back(i);}); 
    std::thread t3([&](){for (int i = 0; i < 1000000; ++i) v->push_back(i);}); 
    std::thread t4([&](){for (int i = 0; i < 1000000; ++i) v->push_back(i);}); 

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    EXPECT_EQ(4000000, v->size());
}
