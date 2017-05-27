#include "benchmark/benchmark_api.h"
#include "haisu/json.h"
#include "gason.h"

std::string json = "{\"a\" : { \"b\" : { \"c\" : { \"d\" : { \"e\" : { \"f\" : \"g\"} } } } } }";

class json_parser : public haisu::json::parser<json_parser>
{
};

static void bench_gason_json(benchmark::State& state) 
{
	JsonValue value;
	JsonAllocator alloc;
	while (state.KeepRunning())
	{
		char *endptr;
		std::string j = json;
		jsonParse(&j[0], &endptr, &value, alloc);
	}
}

static void bench_haisu_json(benchmark::State& state)
{
	json_parser parser;
	while (state.KeepRunning())
	{
		std::string j = json;
		parser.parse(j.c_str());
	}
}

static void bench_gason_empty_json(benchmark::State& state) 
{
	JsonValue value;
	JsonAllocator alloc;
	while (state.KeepRunning())
	{
		std::string j = "{}";
		char *endptr;
		jsonParse(&j[0], &endptr, &value, alloc);
	}
}

static void bench_haisu_empty_json(benchmark::State& state)
{
	json_parser parser;
	while (state.KeepRunning())
	{
		std::string json = "{}";
		parser.parse(json.c_str());
	}
}
BENCHMARK(bench_gason_json);
BENCHMARK(bench_haisu_json);
BENCHMARK(bench_gason_empty_json);
BENCHMARK(bench_haisu_empty_json);
