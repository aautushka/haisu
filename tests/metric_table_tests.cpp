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

