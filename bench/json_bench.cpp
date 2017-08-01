#include "benchmark/benchmark.h"
#include "haisu/json.h"
#include "gason.h"
#include "js0n/js0n.h"

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

struct js0n_parser
{
    void parse(const char* str)
    {
        size_t vlen{};
        js0n("z", 1, str, strlen(str), &vlen);
    }
};

static void bench_gason(benchmark::State& state, std::string json) 
{    
    gason_parser parser;
    std::string j;
    while (state.KeepRunning())
    {
        j = json;
        parser.parse(&j[0]);
    }
}

static void bench_haisu(benchmark::State& state, std::string json)
{
    json_parser parser;
    std::string j;
    while (state.KeepRunning())
    {
        j = json;
        parser.parse(j.c_str());
    }
}

static void bench_js0n(benchmark::State& state, std::string json)
{
    js0n_parser parser;
    std::string j;
    while (state.KeepRunning())
    {
        j = json;
        parser.parse(j.c_str());
    }
}

BENCHMARK_CAPTURE(bench_gason, gason_nested_json, nested_json);
BENCHMARK_CAPTURE(bench_haisu, haisu_nested_json, nested_json);
BENCHMARK_CAPTURE(bench_js0n, js0n_nested_json, nested_json);

BENCHMARK_CAPTURE(bench_gason, gason_deep_json, deep_json);
BENCHMARK_CAPTURE(bench_haisu, haisu_deep_json, deep_json);
BENCHMARK_CAPTURE(bench_js0n, js0n_deep_json, deep_json);

BENCHMARK_CAPTURE(bench_gason, gason_empty_json, empty_json);
BENCHMARK_CAPTURE(bench_haisu, haisu_empty_json, empty_json);
BENCHMARK_CAPTURE(bench_js0n, js0n_empty_json, deep_json);

BENCHMARK_CAPTURE(bench_gason, gason_flat_json, flat_json);
BENCHMARK_CAPTURE(bench_haisu, haisu_flat_json, flat_json);
BENCHMARK_CAPTURE(bench_js0n, js0n_flat_json, flat_json);

BENCHMARK_CAPTURE(bench_gason, gason_flat_array, flat_array);
BENCHMARK_CAPTURE(bench_haisu, haisu_flat_array, flat_array);
BENCHMARK_CAPTURE(bench_js0n, js0n_flat_array, flat_array);

BENCHMARK_CAPTURE(bench_gason, gason_long_names, long_names);
BENCHMARK_CAPTURE(bench_haisu, haisu_long_names, long_names);
BENCHMARK_CAPTURE(bench_js0n, js0n_long_names, long_names);

BENCHMARK_CAPTURE(bench_gason, gason_array_of_arrays, array_of_arrays);
BENCHMARK_CAPTURE(bench_haisu, haisu_array_of_arrays, array_of_arrays);
BENCHMARK_CAPTURE(bench_js0n, js0n_array_of_arrays, array_of_arrays);

BENCHMARK_CAPTURE(bench_gason, gason_array_of_objects, array_of_objects);
BENCHMARK_CAPTURE(bench_haisu, haisu_array_of_objects, array_of_objects);
BENCHMARK_CAPTURE(bench_js0n, js0n_array_of_objects, array_of_objects);

BENCHMARK_CAPTURE(bench_gason, gason_literals, literals);
BENCHMARK_CAPTURE(bench_haisu, haisu_literals, literals);
BENCHMARK_CAPTURE(bench_js0n, js0n_literals, literals);
