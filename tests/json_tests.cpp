/*
MIT License

Copyright (c) 2017 Anton Autushka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <gtest/gtest.h>

#include "haisu/json.h"
#include "haisu/tree.h"

class object : public haisu::json::parser<object>
{
public:
    void on_key(const char* str, const char* end)
    {
        path_.back() = std::string(str, end);
    }

    void on_value(const char* str, const char* end)
    {
        tree_[path_] = std::string(str, end);
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
    void on_array(const char* first, const char* last)
    {
        _arr.push_back(std::string(first, last));
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

struct json_test : ::testing::Test
{
    using path = std::vector<std::string>;
    object json;
    array arr;

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

/*TEST_F(json_test, skips_null_value)
{
    arr.parse("[null,'a','b']");

    EXPECT_EQ("a", arr[0]);
    EXPECT_EQ("b", arr[1]);
}

TEST_F(json_test, skips_true_value)
{
    arr.parse("[true,'a']");

    EXPECT_EQ("a", arr[0]);
}

TEST_F(json_test, skips_false_value)
{
    arr.parse("[false,'a']");

    EXPECT_EQ("a", arr[0]);
}*/

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

