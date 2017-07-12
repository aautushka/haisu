#include "benchmark/benchmark_api.h"
#include "haisu/mono_hash.h"

#include <random>
#include <set>
#include <unordered_map>

#define HASH_SIZE 512

static std::vector<int> generate_random_data()
{
    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::uniform_int_distribution<> dis;
    std::set<int> ret;

    while (ret.size() < HASH_SIZE)
    {
        ret.insert(dis(gen));
    }

    return std::vector<int>(ret.begin(), ret.end());
}

static std::vector<int> generate_sequence()
{
    auto ret = std::vector<int>{};
    for (int i = 0; i < HASH_SIZE; ++i)
    {
        ret.push_back(i);
    }
    return ret;
}

static void bench_collisions(benchmark::State& state) 
{
    auto data = generate_random_data();

    while (state.KeepRunning())
    {
        haisu::mono::hash<int, int, HASH_SIZE, haisu::mono::collide_hash<512>> hash; 

        for (auto i : data)
        {
            hash.insert(i, i);
        }
    }
}

static void bench_std_hash(benchmark::State& state) 
{
    auto data = generate_random_data();

    while (state.KeepRunning())
    {
        haisu::mono::hash<int, int, HASH_SIZE> hash; 

        for (auto i : data)
        {
            hash.insert(i, i);
        }
    }
}

static void bench_std_map(benchmark::State& state) 
{
    auto data = generate_random_data();

    while (state.KeepRunning())
    {
        std::map<int, int> hash;

        for (auto i : data)
        {
            hash.insert(std::make_pair(i, i));
        }
    }
}

static void bench_std_unordered_map(benchmark::State& state) 
{
    auto data = generate_random_data();

    while (state.KeepRunning())
    {
        std::unordered_map<int, int> hash;

        for (auto i : data)
        {
            hash.insert(std::make_pair(i, i));
        }
    }
}

static void bench_best_case_mono_hash(benchmark::State& state) 
{
    auto data = generate_sequence();

    while (state.KeepRunning())
    {
        using namespace haisu::mono;
        hash<int, int, HASH_SIZE, direct_hash<int>> hash; 

        for (auto i : data)
        {
            hash.insert(i, i);
        }
    }
}

static void bench_amiga_hash(benchmark::State& state) 
{
    auto data = generate_random_data();

    while (state.KeepRunning())
    {
        using namespace haisu::mono;
        hash<int, int, HASH_SIZE, direct_hash<int>> hash; 

        for (auto i : data)
        {
            hash.insert(i, i);
        }
    }
}

static void bench_direct_hash_on_random_data(benchmark::State& state) 
{
    auto data = generate_random_data();

    while (state.KeepRunning())
    {
        using namespace haisu::mono;
        hash<int, int, HASH_SIZE, direct_hash<int>> hash; 

        for (auto i : data)
        {
            hash.insert(i, i);
        }
    }
}

BENCHMARK(bench_collisions);
BENCHMARK(bench_std_hash);
BENCHMARK(bench_std_map);
BENCHMARK(bench_std_unordered_map);
BENCHMARK(bench_best_case_mono_hash);
BENCHMARK(bench_amiga_hash);
BENCHMARK(bench_direct_hash_on_random_data);
