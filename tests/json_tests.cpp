#include <gtest/gtest.h>

#include "haisu/json.h"

struct json_test : ::testing::Test
{
	haisu::json::model json;
};

TEST_F(json_test, parses_json_having_one_pair)
{
	json.parse("{\"a\":\"b\"}");
	EXPECT_EQ("b", json["a"]);
}

TEST_F(json_test, parses_multiple_json_values)
{
	json.parse("{\"a\":\"b\",\"c\":\"d\"}");

	EXPECT_EQ("b", json["a"]);
	EXPECT_EQ("d", json["c"]);
}

TEST_F(json_test, parses_nested_object)
{
	json.parse("{\"a\":{\"b\":\"c\"}}");

	EXPECT_EQ("c", json["b"]);
}

TEST_F(json_test, ignores_blanks_when_parsing)
{
	json.parse(" { \"a\" : \"b\" } ");
	EXPECT_EQ("b", json["a"]);
}

