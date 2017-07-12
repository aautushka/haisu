#include "benchmark/benchmark.h"
#include "haisu/json.h"
#include "gason.h"

std::string nested_json = "{\"a\" : { \"b\" : { \"c\" : { \"d\" : { \"e\" : { \"f\" : \"g\"} } } } } }";
std::string flat_json = "{\"a\" : \"b\", \"c\" : \"d\", \"e\" : \"f\", \"g\" : \"h\", \"i\" : \"j\", \"k\" : \"l\", \"m\" : \"n\"}";
std::string flat_array = "[\"a\", \"b\", \"c\", \"d\", \"e\", \"f\", \"g\", \"h\", \"i\", \"j\", \"k\"]";
std::string long_names = "{\"aaaaaaaaaaaaaaaaa\" : { \"bbbbbbbbbbbbb\" : { \"cccccccccccccccc\" : { \"ddddddddddddddddd\"}}}}";
std::string empty_json = "{}";

class gason_parser
{
public:
    void parse(char * str)
    {
        char *endptr;
        jsonParse(str, &endptr, &value, alloc);
    }
private:
    JsonValue value;
    JsonAllocator alloc;
};

class json_parser : public haisu::json::parser<json_parser>
{
};


static void bench_gason_json(benchmark::State& state, std::string json) 
{    
    gason_parser parser;
    while (state.KeepRunning())
    {
        std::string j = json;
        parser.parse(&j[0]);
    }
}

static void bench_haisu_json(benchmark::State& state, std::string json)
{
    json_parser parser;
    while (state.KeepRunning())
    {
        std::string j = json;
        parser.parse(j.c_str());
    }
}

BENCHMARK_CAPTURE(bench_gason_json, gason_nested_json, nested_json);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_nested_json, nested_json);

BENCHMARK_CAPTURE(bench_gason_json, gason_empty_json, empty_json);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_empty_json, empty_json);

BENCHMARK_CAPTURE(bench_gason_json, gason_flat_json, flat_json);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_flat_json, flat_json);

BENCHMARK_CAPTURE(bench_gason_json, gason_flat_array, flat_array);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_flat_array, flat_array);

BENCHMARK_CAPTURE(bench_gason_json, gason_long_names, long_names);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_long_names, long_names);

