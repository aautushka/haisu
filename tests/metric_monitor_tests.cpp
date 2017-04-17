#include <gtest/gtest.h>
#include "haisu/metric.h"

struct metric_monitor_test : ::testing::Test
{
	using mon_t = haisu::metric::monitor<int, 16>;
	mon_t mon;

	void busy_loop(int usec)
	{
		using haisu::metric::timer;
		auto start = timer::now();
		while (timer::now() - start < usec);
	}
};

TEST_F(metric_monitor_test, creates_manual_metric)
{
	mon.start(1);
	busy_loop(1);	
	mon.stop();

	auto rep = mon.report();
	EXPECT_LE(1, rep[1]);
}

TEST_F(metric_monitor_test, creates_nested_metrics)
{
	mon.start(1);
	mon.start(2);
	busy_loop(1);	
	mon.stop();
	mon.stop();

	auto rep = mon.report();
	EXPECT_LE(1, rep[1]);

	std::vector<int> key = {1, 2};
	EXPECT_LE(1, rep[key]);
}

TEST_F(metric_monitor_test, creates_scoped_metric)
{
	{
	auto metric = mon.scope(1);
	busy_loop(1);	
	}

	auto rep = mon.report();
	EXPECT_LE(1, rep[1]);
}

TEST_F(metric_monitor_test, ignores_overflow_metrics)
{
	haisu::metric::monitor<int, 1> mon;

	{
	auto metric1 = mon.scope(1);
	auto metric2 = mon.scope(2);
	auto metric3 = mon.scope(3);
	busy_loop(1);	
	}

	auto rep = mon.report();
	EXPECT_EQ(1, rep.size());
	EXPECT_LE(1, rep[1]);
}
