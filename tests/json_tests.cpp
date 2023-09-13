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

// clang-format off

#include <gtest/gtest.h>

#include "haisu/json.h"
#include "haisu/tree.h"
    
using string_literal = haisu::json::string_literal;
using bool_literal = haisu::json::bool_literal;
using null_literal = haisu::json::null_literal;
using numeric_literal = haisu::json::numeric_literal;

class object : public haisu::json::parser<object>
{
public:
    void on_key(string_literal lit)
    {
        path_.back() = std::string(lit.view.begin(), lit.view.end());
    }

    void on_value(string_literal lit)
    {
        tree_[path_] = std::string(lit.view.begin(), lit.view.end());
    }

    void on_value(bool_literal lit)
    {
        tree_[path_] = lit.value ? "boolean true" : "boolean false";
    }

    void on_value(numeric_literal lit)
    {
        tree_[path_] = std::string(lit.view.begin(), lit.view.end());
    }

    void on_value(null_literal lit)
    {
        tree_[path_] = "null literal";
    }

    void on_new_object()
    {
        path_.push_back(std::string());
    }

    void on_object_end()
    {
        path_.pop_back();
    }

    std::string operator [](const std::string& key) const
    {
        return tree_[key];
    }

    std::string operator [](const char* key) const
    {
        return tree_[std::string(key)];
    }

    template <typename T>
    std::string operator [](const T& t) const
    {
        return tree_[t];
    }

private:
    std::vector<std::string> path_;
    haisu::tree<std::string, std::string> tree_;
};

class array : public haisu::json::parser<array>
{
public:

    void on_array(string_literal lit)
    {
        _arr.emplace_back(lit.view.begin(), lit.view.end());
    }

    void on_array(bool_literal lit)
    {
        _arr.push_back(lit.value ? "boolean true" : "boolean false");
    }

    void on_array(null_literal lit)
    {
        _arr.push_back("null literal");
    }

    void on_array(numeric_literal lit)
    {
        _arr.push_back(std::string(lit.view.begin(), lit.view.end()));
    }

    size_t size() const
    {
        return _arr.size();
    }

    std::string operator [](int i) const
    {
        return _arr[i];
    }

private:
    std::vector<std::string> _arr;
};

struct error_counter : public haisu::json::parser<error_counter>
{
    void on_error(haisu::json::error err)
    {
        ++error_count;
    }

    bool has_errors() const
    {
        return error_count > 0;
    }

    int error_count{};
};

struct json_test : ::testing::Test
{
    using path = std::vector<std::string>;
    object json;
    array arr;
    error_counter err;
};

TEST_F(json_test, parses_json_having_one_pair)
{
    json.parse("{\"a\":\"b\"}");
    EXPECT_EQ("b", json["a"]);
}

TEST_F(json_test, parses_nested_object)
{
    json.parse("{\"a\":{\"b\":\"c\"}}");

    auto p = path{"a", "b"};
    EXPECT_EQ("c", json[p]);
}

TEST_F(json_test, parses_multiple_json_values)
{
    json.parse("{\"a\":\"b\",\"c\":\"d\"}");

    EXPECT_EQ("b", json["a"]);
    EXPECT_EQ("d", json["c"]);
}

TEST_F(json_test, parses_same_level_nested_objs)
{
    json.parse("{\"a\" : {\"b\":\"c\"}, \"d\" : {\"e\":{\"f\":\"g\"}}}");

    auto p1 = path{"a", "b"};
    auto p2 = path{"d", "e", "f"};

    EXPECT_EQ("c", json[p1]);
    EXPECT_EQ("g", json[p2]);
}

TEST_F(json_test, ignores_blanks_when_parsing)
{
    json.parse(" { \"a\" : \"b\" } ");
    EXPECT_EQ("b", json["a"]);
}

TEST_F(json_test, parses_array_of_single_item)
{
    arr.parse("[\"a\"]");
    EXPECT_EQ("a", arr[0]);
}

TEST_F(json_test, reads_array_of_many_items)
{
    arr.parse("[\"a\", \"b\"]");

    EXPECT_EQ("a", arr[0]);
    EXPECT_EQ("b", arr[1]);
}

TEST_F(json_test, skips_blanks_when_reading_array)
{
    arr.parse(" [ \"a\" , \"b\" ] ");

    EXPECT_EQ("a", arr[0]);
    EXPECT_EQ("b", arr[1]);
}

TEST_F(json_test, reads_array_inside_of_object)
{
    arr.parse("{\"a\": [\"b\"]}");

    EXPECT_EQ("b", arr[0]);
}

TEST_F(json_test, reads_object_embedded_in_array)
{
    json.parse("[[[{\"a\": \"b\"}]]]");

    EXPECT_EQ("b", json["a"]);
}

TEST_F(json_test, parses_partial_json)
{
    json.parse("{\"a\":\"b\", ");
    EXPECT_EQ("b", json["a"]);
}

TEST_F(json_test, parses_single_quoted_strings)
{
    json.parse("{'a':'b'}");
    EXPECT_EQ("b", json["a"]);
}

TEST_F(json_test, skips_null_value)
{
    arr.parse("[null,'a','b']");

    EXPECT_EQ("a", arr[1]);
    EXPECT_EQ("b", arr[2]);
}

TEST_F(json_test, reads_boolean_true_in_array)
{
    arr.parse("[true,'a']");

    EXPECT_EQ("boolean true", arr[0]);
}

TEST_F(json_test, reads_boolean_false_in_array)
{
    arr.parse("[false,'a']");

    EXPECT_EQ("boolean false", arr[0]);
}

TEST_F(json_test, understands_escaped_quote_character_inside_of_string)
{
    json.parse("{'a\\'c':'b'}");
    EXPECT_EQ("b", json["a\\'c"]);
}

TEST_F(json_test, does_not_confuse_escaped_backslash_with_quote)
{
    json.parse("{'a\\\\':'b'}");
    EXPECT_EQ("b", json["a\\\\"]);
}

TEST_F(json_test, parses_multiple_escape_sequences)
{
    json.parse("{'a\\\\\\'c':'b'}");
    EXPECT_EQ("b", json["a\\\\\\'c"]);
}

TEST_F(json_test, reads_boolean_true_in_object)
{
    json.parse("{'a': true}");
    EXPECT_EQ("boolean true", json["a"]);
}

TEST_F(json_test, reads_boolean_false_in_object)
{
    json.parse("{'a': false}");
    EXPECT_EQ("boolean false", json["a"]);
}

TEST_F(json_test, reads_null_literal_from_object)
{
    json.parse("{'a': null}");
    EXPECT_EQ("null literal", json["a"]);
}

TEST_F(json_test, reads_null_literal_from_array)
{
    arr.parse("[null]");
    ASSERT_EQ(1, arr.size());
    EXPECT_EQ("null literal", arr[0]);
}

TEST_F(json_test, signals_unexpected_character_error)
{
    err.parse("abc");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, no_errors_found)
{
    err.parse("{}");
    EXPECT_FALSE(err.has_errors());
}

TEST_F(json_test, invalid_true_literal_error)
{
    err.parse("[truee]");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, invalid_false_literal)
{
    err.parse("[falses]");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, invalid_null_literal)
{
    err.parse("[nulll]");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, imcomplete_json_error)
{
    err.parse("[null, ");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, malformed_object_error)
{
    err.parse("{}}");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, malformed_array_error)
{
    err.parse("{}}");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, invalid_number_literal)
{
    err.parse("[1234aadf]");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, incomplete_string_literal_error)
{
    err.parse("['hello");
    EXPECT_TRUE(err.has_errors());
}

TEST_F(json_test, terminates_json_parser_middle_way)
{
    struct parser : public haisu::json::parser<parser>
    {
        void on_array(string_literal lit)
        {
            ++array_size;
            terminate();
        }

        int array_size = 0;
    };

    parser p;
    p.parse("['hello', 'world']");

    EXPECT_EQ(1, p.array_size);
}

TEST_F(json_test, signals_error_if_does_not_have_enough_memory)
{
    struct parser : public haisu::json::parser<parser, 1>
    {
        void on_error(haisu::json::error err)
        {
            ++error_count;
        }
        int error_count = 0;
    };

    parser p;
    p.parse("[['hello']]");

    EXPECT_EQ(1, p.error_count);
}

TEST_F(json_test, parses_numeric_literal_in_object)
{
    json.parse("{'a':123456}");
    EXPECT_EQ("123456", json["a"]);
}

TEST_F(json_test, parses_numeric_literal_in_array)
{
    arr.parse("[123456]");
    EXPECT_EQ("123456", arr[0]);
}

TEST_F(json_test, collect_keys) {
    struct parser: haisu::json::parser<parser> 
    {
        void on_key(haisu::json::string_literal key) {
            keys.append(key.view);
        }

        std::string operator()(const char* str) {
            keys = {};
            parse(str);
            return std::move(keys);
        }

        std::string keys;
    };

    parser parse;

    // key - key,val - key - key,val - key - bad
    EXPECT_EQ("ab", parse(R"({"a":0,"b":1})"));
    EXPECT_EQ("ab", parse(R"({"a":"0","b":"1"})"));
    EXPECT_EQ("ab", parse(R"({"a":null,"b":null})"));
    EXPECT_EQ("ab", parse(R"({"a":true,"b":true})"));
    EXPECT_EQ("ab", parse(R"({"a":false,"b":false})"));
    EXPECT_EQ("ab", parse(R"({"a":{},"b":{}})"));
    EXPECT_EQ("ab", parse(R"({"a":[],"b":[]})"));
    EXPECT_EQ("ab", parse(R"({"a":[[]],"b":[[]]})"));
    EXPECT_EQ("ab", parse(R"({"a":[[[]]],"b":[[[]]]})"));

    // key - key,key - key,key,val - key, key - key -
    EXPECT_EQ("ab", parse(R"({"a": {"b": 0}})"));
    EXPECT_EQ("ab", parse(R"({"a": {"b": "0"}})"));
    EXPECT_EQ("ab", parse(R"({"a": {"b": false}})"));
    EXPECT_EQ("ab", parse(R"({"a": {"b": true}})"));
    EXPECT_EQ("ab", parse(R"({"a": {"b": null}})"));
    EXPECT_EQ("ab", parse(R"({"a": {"b": {}}})"));
    EXPECT_EQ("ab", parse(R"({"a": {"b": []}})"));

    EXPECT_EQ("ac", parse(R"({"a": {}, "c": 1})"));
    EXPECT_EQ("ac", parse(R"({"a": {}, "c": "0"})"));
    EXPECT_EQ("ac", parse(R"({"a": {}, "c": false})"));
    EXPECT_EQ("ac", parse(R"({"a": {}, "c": true})"));
    EXPECT_EQ("ac", parse(R"({"a": {}, "c": []})"));
    EXPECT_EQ("ac", parse(R"({"a": {}, "c": {}})"));

    EXPECT_EQ("abc", parse(R"({"a": {"b": 0}, "c": 1})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": null}, "c": null})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": "0"}, "c": "1"})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": true}, "c": true})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": false}, "c": false})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": {}}, "c": true})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": {}}, "c": false})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": []}, "c": true})"));
    EXPECT_EQ("abc", parse(R"({"a": {"b": []}, "c": false})"));

    EXPECT_EQ("", parse(R"(["a", "b"])"));
    EXPECT_EQ("", parse(R"(["a", ["b"]])"));
    EXPECT_EQ("", parse(R"([["b"], "c"])"));
    EXPECT_EQ("", parse(R"([{}, {}])"));
    EXPECT_EQ("ab", parse(R"([{"a": 0}, {"b": 1}])"));
    EXPECT_EQ("ab", parse(R"([[{"a": 0}], [{"b": 1}]])"));

    EXPECT_EQ("ab", parse(R"([{"a":0,"b":1}])"));
    EXPECT_EQ("ab", parse(R"([{"a":"0","b":"1"}])"));
    EXPECT_EQ("ab", parse(R"([{"a":null,"b":null}])"));
    EXPECT_EQ("ab", parse(R"([{"a":true,"b":true}])"));
    EXPECT_EQ("ab", parse(R"([{"a":false,"b":false}])"));
    EXPECT_EQ("ab", parse(R"([{"a":{},"b":{}}])"));
    EXPECT_EQ("ab", parse(R"([{"a":[],"b":[]}])"));
    EXPECT_EQ("ab", parse(R"([{"a":[[]],"b":[[]]}])"));
    EXPECT_EQ("ab", parse(R"([{"a":[[[]]],"b":[[[]]]}])"));

    auto keys = parse(R"({"a":0,"b":"0","c":[{"d":1, "e":{"f":2,"g":3},"h":4}, "i"], "j":5})");
    EXPECT_EQ("abcdefghj", keys);
}
