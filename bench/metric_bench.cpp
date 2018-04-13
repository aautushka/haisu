#include "benchmark/benchmark.h"
#include "haisu/metric.h"

static void bench_original_metrics(benchmark::State& state) 
{
    using namespace haisu::metric;
    monitor<int, 4> mon;
    while (state.KeepRunning())
    {
        mon.start(1);
        mon.start(2);
        mon.start(3);
        mon.start(4);
        mon.stop();
        mon.stop();
        mon.stop();
        mon.stop();
    }
}

static void bench_trie_metrics(benchmark::State& state) 
{
    namespace met = haisu::metric;
    met::trie<int, met::timer> mon;
    while (state.KeepRunning())
    {
        mon.down(1);
        mon.down(2);
        mon.down(3);
        mon.down(4);
        mon.up();
        mon.up();
        mon.up();
        mon.up();
    }
}

BENCHMARK(bench_original_metrics);
BENCHMARK(bench_trie_metrics);
