#include "benchmark/benchmark.h"
#include "haisu/metric.h"

static void bench_original_metrics(benchmark::State& state) 
{
    using namespace haisu::metric;
    monitor<int, 16> mon;
    while (state.KeepRunning())
    {
        mon.start(1);
        mon.start(2);
        mon.start(3);
        mon.start(4);
        mon.start(5);
        mon.start(6);
        mon.start(7);
        mon.start(8);
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
    }
}

static void bench_trie_metrics(benchmark::State& state) 
{
    namespace met = haisu::metric;
    met::monitor2<int> mon;
    while (state.KeepRunning())
    {
        mon.start(1);
        mon.start(2);
        mon.start(3);
        mon.start(4);
        mon.start(5);
        mon.start(6);
        mon.start(7);
        mon.start(8);
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
    }
}

BENCHMARK(bench_original_metrics);
BENCHMARK(bench_trie_metrics);