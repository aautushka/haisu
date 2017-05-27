#include <gtest/gtest.h>

#include "haisu/json.h"

class object : public haisu::json::parser<object>
{
public:
	void on_key(const char* str, const char* end)
	{
		key_.assign(str, end);
	}

	void on_value(const char* str, const char* end)
	{
		pairs_[key_] = std::string(str, end);
		key_.clear();
	}

	std::string operator [](const std::string& key) const
	{
		auto i = pairs_.find(key);
		if (i != pairs_.end())
		{
			return i->second;
		}

		return "";
	}

private:
	std::map<std::string, std::string> pairs_;
	std::string key_;
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
	object json;
	array arr;

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
