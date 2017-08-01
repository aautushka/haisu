#include "benchmark/benchmark.h"
#include "haisu/json.h"
#include "gason.h"

std::string nested_json = "{\"a\" : { \"b\" : { \"c\" : { \"d\" : { \"e\" : { \"f\" : \"g\"} } } } } }";
std::string flat_json = "{\"a\" : \"b\", \"c\" : \"d\", \"e\" : \"f\", \"g\" : \"h\", \"i\" : \"j\", \"k\" : \"l\", \"m\" : \"n\"}";
std::string flat_array = "[\"a\", \"b\", \"c\", \"d\", \"e\", \"f\", \"g\", \"h\", \"i\", \"j\", \"k\"]";
std::string long_names = "{\"aaaaaaaaaaaaaaaaa\" : { \"bbbbbbbbbbbbb\" : { \"cccccccccccccccc\" : { \"ddddddddddddddddd\"}}}}";
std::string empty_json = "{}";
std::string deep_json = "{\"a\":{\"b\":{\"c\":{\"d\":{\"e\":{\"f\":{\"j\":{\"h\":{\"i\":{\"j\":{\"k\":{\"l\":{\"m\":{\"n\":{\"o\":{\"p\":{\"q\":{\"r\":\"s\"}}}}}}}}}}}}}}}}}}";
std::string array_of_arrays = "[[\"a\"], [\"b\"], [\"c\"], [\"d\"],[\"e\"],[\"f\"],[\"g\"],[\"h\"]]";
std::string array_of_objects = "[{\"a\":\"b\"},{\"c\":\"d\"},{\"e\":\"f\"},{\"g\":\"h\"},{\"i\",\"j\"},{\"k\":\"l\"},{\"m\":\"n\"}]";
std::string literals = "[true, false, true, null, null, true, false, null, true, false, null, true, false, null]";

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

struct json_parser : public haisu::json::parser<json_parser>
{
    template <typename Literal>
    void on_value(Literal&&)
    {
        ++literals;
    }

    template <typename Literal>
    void on_array(Literal&&)
    {
        ++literals;
    }

    template <typename Literal>
    void on_key(Literal&&)
    {
        ++literals;
    }

    int literals{};
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

BENCHMARK_CAPTURE(bench_gason_json, gason_deep_json, deep_json);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_deep_json, deep_json);

BENCHMARK_CAPTURE(bench_gason_json, gason_empty_json, empty_json);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_empty_json, empty_json);

BENCHMARK_CAPTURE(bench_gason_json, gason_flat_json, flat_json);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_flat_json, flat_json);

BENCHMARK_CAPTURE(bench_gason_json, gason_flat_array, flat_array);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_flat_array, flat_array);

BENCHMARK_CAPTURE(bench_gason_json, gason_long_names, long_names);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_long_names, long_names);

BENCHMARK_CAPTURE(bench_gason_json, gason_array_of_arrays, array_of_arrays);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_array_of_arrays, array_of_arrays);

BENCHMARK_CAPTURE(bench_gason_json, gason_array_of_objects, array_of_objects);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_array_of_objects, array_of_objects);

BENCHMARK_CAPTURE(bench_gason_json, gason_literals, literals);
BENCHMARK_CAPTURE(bench_haisu_json, haisu_literals, literals);
