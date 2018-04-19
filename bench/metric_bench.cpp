#include "benchmark/benchmark.h"
#include "haisu/metric.h"
#include "haisu/metric2.h"

static void bench_original_metrics(benchmark::State& state) 
{
    using namespace haisu::metric;
    monitor<int8_t, 16> mon;
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
    namespace met = haisu::metric2;
    met::monitor<int8_t> mon;
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

static void bench_original_metrics_shallow(benchmark::State& state) 
{
    using namespace haisu::metric;
    monitor<int8_t, 16> mon;
    while (state.KeepRunning())
    {
        mon.start(1);
        mon.stop();
    }
}

static void bench_trie_metrics_shallow(benchmark::State& state) 
{
    namespace met = haisu::metric2;
    met::monitor<int8_t> mon;
    while (state.KeepRunning())
    {
        mon.start(1);
        mon.stop();
    }
}

static void bench_original_metrics_flat_hierarchy(benchmark::State& state) 
{
    using namespace haisu::metric;
    monitor<int8_t, 16> mon;
    mon.start(1);
    while (state.KeepRunning())
    {
        mon.start(2);
        mon.stop();
        mon.start(3);
        mon.stop();
        mon.start(4);
        mon.stop();
        mon.start(5);
        mon.stop();
    }
}

static void bench_trie_metrics_flat_hierarchy(benchmark::State& state) 
{
    namespace met = haisu::metric2;
    met::monitor<int8_t> mon;
    mon.start(1);
    while (state.KeepRunning())
    {
        mon.start(2);
        mon.stop();
        mon.start(3);
        mon.stop();
        mon.start(4);
        mon.stop();
        mon.start(5);
        mon.stop();
    }
}


BENCHMARK(bench_original_metrics);
BENCHMARK(bench_trie_metrics);
BENCHMARK(bench_original_metrics_shallow);
BENCHMARK(bench_trie_metrics_shallow);
BENCHMARK(bench_original_metrics_flat_hierarchy);
BENCHMARK(bench_trie_metrics_flat_hierarchy);
