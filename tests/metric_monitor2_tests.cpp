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

struct metric_monitor2_test : ::testing::Test
{
    using mon_t = haisu::metric::monitor2<int>;
    mon_t mon;

    void busy_loop(int usec)
    {
        using haisu::metric::timer;
        auto start = timer::now();
        while (timer::now() - start < usec);
    }

};

TEST_F(metric_monitor2_test, creates_manual_metric)
{
    mon.start(1);
    busy_loop(1);    
    mon.stop();

    auto rep = mon.report();
    EXPECT_LE(1, rep[1]);
}

TEST_F(metric_monitor2_test, creates_nested_metrics)
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

TEST_F(metric_monitor2_test, creates_scoped_metric)
{
    {
    auto metric = mon.scope(1);
    busy_loop(1);    
    }

    auto rep = mon.report();
    EXPECT_LE(1, rep[1]);
}

TEST_F(metric_monitor2_test, produces_json_report)
{
    auto rep = mon.report_json();
    EXPECT_EQ("{}", mon.report_json());
}
