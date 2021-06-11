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
#include "haisu/metric2.h"

struct metric_monitor2_test : ::testing::Test
{
    using mon_t = haisu::metric2::monitor<int>;
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
