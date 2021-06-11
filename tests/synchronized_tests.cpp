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
