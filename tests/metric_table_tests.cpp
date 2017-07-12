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
#include "haisu/metric.h"

struct metric_table_test : ::testing::Test
{
	using table_t = haisu::metric::table<int, 16>;
	using key_t = table_t::key_type;
	table_t table;
};

TEST_F(metric_table_test, adds_single_measure)
{
	table.store({1, 2});
	auto q = table.query();
	EXPECT_EQ(2, q[1]);
}

TEST_F(metric_table_test, accumulates_measurements)
{
	table.store({1, 2});
	table.store({1, 2});
	auto q = table.query();
	EXPECT_EQ(4, q[1]);
}

TEST_F(metric_table_test, adds_measure_with_a_rather_long_path)
{
	key_t key{1, 2, 3};
	table.store(key, {3, 1});

	auto q = table.query();
	EXPECT_EQ(1, q[key]); 
}

TEST_F(metric_table_test, accumulates_measurements_using_a_long_key)
{
	key_t key{1, 2, 3};
	table.store(key, {3, 1});
	table.store(key, {3, 2});

	auto q = table.query();
	EXPECT_EQ(3, q[key]); 
}

