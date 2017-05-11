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
