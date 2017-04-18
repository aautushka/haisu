#include <gtest/gtest.h>
#include "haisu/mono.h"

struct mono_string_test : ::testing::Test
{
	using string = haisu::mono::string<15>;
	string str;
};

TEST_F(mono_string_test, creates_empty_stack)
{
	EXPECT_TRUE(str.empty());
	EXPECT_EQ(0, str.size());
}

TEST_F(mono_string_test, creates_non_empty_string)
{
	string str("hello");
	EXPECT_FALSE(str.empty());
	EXPECT_NE(0, str.size());
}

TEST_F(mono_string_test, gets_c_string)
{
	str.assign("hello");
	EXPECT_STREQ("hello", str.c_str());
	EXPECT_STREQ("hello", str.data());
}

TEST_F(mono_string_test, gets_string_size)
{
	str.assign("hello");
	EXPECT_EQ(5, str.size());
}

TEST_F(mono_string_test, gets_string_capacity)
{
	haisu::mono::string<4> str;
	EXPECT_EQ(4, str.capacity());
	EXPECT_EQ(4, str.max_size());
}

TEST_F(mono_string_test, fills_string_to_full_capacity)
{
	haisu::mono::string<5> str;
	str.assign("hello");
	EXPECT_EQ(5, str.size());
	EXPECT_STREQ("hello", str.c_str());
}

TEST_F(mono_string_test, compares_equal_strings)
{
	EXPECT_TRUE(string("hello") == string("hello"));
	EXPECT_FALSE(string("hello") != string("hello"));
	EXPECT_FALSE(string("hello") < string("hello"));
	EXPECT_FALSE(string("hello") > string("hello"));
	EXPECT_TRUE(string("hello") <= string("hello"));
	EXPECT_TRUE(string("hello") >= string("hello"));
}

TEST_F(mono_string_test, compares_different_strings)
{
	EXPECT_FALSE(string("hello") == string("world"));
	EXPECT_TRUE(string("hello") != string("world"));
	EXPECT_TRUE(string("hello") < string("world"));
	EXPECT_FALSE(string("hello") > string("world"));
	EXPECT_TRUE(string("hello") <= string("world"));
	EXPECT_FALSE(string("hello") >= string("world"));
}

TEST_F(mono_string_test, constructs_from_c_string)
{
	string str("hello");

	EXPECT_EQ(5, str.size());
	EXPECT_STREQ("hello", str.c_str());
}

TEST_F(mono_string_test, constructs_from_other_string)
{
	EXPECT_EQ(string("hello"), string(string("hello")));
}

TEST_F(mono_string_test, constructs_string_from_char)
{
	EXPECT_EQ(string("hhhhh"), string(5, 'h'));
}

TEST_F(mono_string_test, constructs_from_c_string_with_explicit_length)
{
	EXPECT_EQ(string("hello"), string("hello world", 5));
}

TEST_F(mono_string_test, clears_string)
{
	str.assign("hello");
	str.clear();
	EXPECT_EQ(string(), str);
}

TEST_F(mono_string_test, resizes_string)
{
	str.assign("hello world");
	str.resize(5);
	EXPECT_EQ(string("hello"), str);
}

TEST_F(mono_string_test, gets_character_at_index)
{
	str = "hello";
	EXPECT_EQ('h', str[0]);
	EXPECT_EQ('e', str[1]);
}

TEST_F(mono_string_test, gets_last_character)
{
	str = "hello";
	EXPECT_EQ('o', str.back());
}

TEST_F(mono_string_test, gets_first_character)
{
	str = "hello";
	EXPECT_EQ('h', str.front());
}

TEST_F(mono_string_test, assignes_c_string)
{
	str = "hello";
	EXPECT_EQ(string("hello"), str);
}

TEST_F(mono_string_test, assignes_char)
{
	str = 'h';
	EXPECT_EQ(string("h"), str);
}

TEST_F(mono_string_test, appends_c_string)
{
	str.append("hello ");
	str.append("world");

	EXPECT_EQ(string("hello world"), str);
}

TEST_F(mono_string_test, appends_string)
{
	str.append(string("hello "));
	str.append(string("world"));

	EXPECT_EQ(string("hello world"), str);
}

TEST_F(mono_string_test, appends_chars)
{
	str.append(2, 'h');
	str.append(3, 'w');

	EXPECT_EQ(string("hhwww"), str);
}

TEST_F(mono_string_test, appends_c_string_with_known_size)
{
	str.append("hello world", 6);
	str.append("world world", 5);

	EXPECT_EQ(string("hello world"), str);
}

TEST_F(mono_string_test, pushes_back_a_character)
{
	str.push_back('h');
	str.push_back('e');

	EXPECT_EQ(string("he"), str);
}

TEST_F(mono_string_test, pops_a_character_from_back)
{
	str = "hello@";
	str.pop_back();

	EXPECT_EQ(string("hello"), str);
}

TEST_F(mono_string_test, alters_chars_accessed_by_index)
{
	str = "kind";
	str[0] = 'm';
	str.at(2) = 'l';

	EXPECT_EQ(string("mild"), str);
}
