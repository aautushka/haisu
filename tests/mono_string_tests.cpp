#include <gtest/gtest.h>
#include "haisu/mono.h"

struct mono_string_test : ::testing::Test
{
	using string = haisu::mono::string<15>;
	string str;
	string hello_world;

	mono_string_test()
	{
		hello_world.assign("hello world");
	}
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

TEST_F(mono_string_test, assigns_substring)
{
	string other("hello world");
	str.assign(other, 1, 4);

	EXPECT_EQ(string("ello"), str);
}

TEST_F(mono_string_test, the_requested_size_may_point_well_past_beyond_the_end_but_this_is_fine)
{
	string other("hello world");
	str.assign(other, 1, 1000);

	EXPECT_EQ(string("ello world"), str);
}

TEST_F(mono_string_test, assignes_to_the_end_of_the_string_by_default)
{
	string other("hello world");
	str.assign(other, 1);

	EXPECT_EQ(string("ello world"), str);
}

TEST_F(mono_string_test, substrings_string)
{
	EXPECT_EQ(string("ello"), hello_world.substr(1, 4));
	EXPECT_EQ(string("ello world"), hello_world.substr(1));
}

TEST_F(mono_string_test, copies_substring_to_buffer)
{
	char buffer[16] = {0};

	hello_world.copy(buffer, 4, 1);
	EXPECT_STREQ("ello", buffer);
	
	hello_world.copy(buffer, 5);
	EXPECT_STREQ("hello", buffer);

}

TEST_F(mono_string_test, copy_returns_the_number_of_copied_chars)
{
	char buffer[16] = {0};
	EXPECT_EQ(5, hello_world.copy(buffer, 5));
}

TEST_F(mono_string_test, erases_substring)
{
	EXPECT_EQ(string(), string("hello world").erase());
	EXPECT_EQ(string("world"), string("hello world").erase(0, 6));
	EXPECT_EQ(string("hello"), string("hello world").erase(5));
}

TEST_F(mono_string_test, searches_for_substring)
{
	EXPECT_EQ(string::npos, hello_world.find("foobar"));
	EXPECT_EQ(6, hello_world.find("world"));
	EXPECT_EQ(6, hello_world.find("world", 6));
	EXPECT_EQ(string::npos, hello_world.find("world", 7));
}

TEST_F(mono_string_test, searches_for_a_character)
{
	EXPECT_EQ(string::npos, hello_world.find('z'));
	EXPECT_EQ(6, hello_world.find('w'));
	EXPECT_EQ(6, hello_world.find('w', 6));
	EXPECT_EQ(string::npos, hello_world.find('w', 7));
}

TEST_F(mono_string_test, searches_for_substring_with_explicit_length)
{
	EXPECT_EQ(string::npos, hello_world.find("foobar", 0, 6));
	EXPECT_EQ(6, hello_world.find("world!", 0, 5));
	EXPECT_EQ(6, hello_world.find("world!", 6, 5));
	EXPECT_EQ(string::npos, hello_world.find("world!", 7, 5));
}

TEST_F(mono_string_test, inserts_string)
{
	EXPECT_EQ(string("hello world"), string("hello").insert(5, " world"));
	EXPECT_EQ(string("hello world"), string("world").insert(0, "hello "));
	EXPECT_EQ(string("hello world"), string("helloworld").insert(5, " ", 1));

	haisu::mono::string<5> str;
	EXPECT_EQ(string("hello"), str.insert(0, "hello"));
}

TEST_F(mono_string_test, inserts_set_of_chars)
{
	EXPECT_EQ(string("hello wwwww"), string("hello ").insert(6, 5, 'w'));
	EXPECT_EQ(string("hhhhh world"), string(" world").insert(0, 5, 'h'));
	EXPECT_EQ(string("hello world"), string("helloworld").insert(5, 1, ' '));

	haisu::mono::string<5> str;
	EXPECT_EQ(string("xxxxx"), str.insert(0, 5, 'x'));
}

TEST_F(mono_string_test, inserts_substring)
{
	string hello("hello");
	string world("world");
	string helloworld("helloworld");

	EXPECT_EQ(hello_world, hello.insert(5, hello_world, 5)); 
	EXPECT_EQ(hello_world, world.insert(0, hello_world, 0, 6)); 
}

TEST_F(mono_string_test, replaces_substring)
{
	EXPECT_EQ(hello_world, string("hola world").replace(0, 4, "hello"));
	EXPECT_EQ(hello_world, string("hello mundo").replace(6, 5, "world"));
	EXPECT_EQ(hello_world, string("helloworld").replace(5, 0, " "));
}

TEST_F(mono_string_test, replaces_substring_with_a_set_of_chars)
{
	EXPECT_EQ(string("hhhhh world"), string("hola world").replace(0, 4, 5, 'h'));
	EXPECT_EQ(string("hello wwwww"), string("hello mundo").replace(6, 5, 5, 'w'));
	EXPECT_EQ(hello_world, string("helloworld").replace(5, 0, 1, ' '));
}

TEST_F(mono_string_test, finds_last_character)
{
	EXPECT_EQ(0, hello_world.rfind('h'));
	EXPECT_EQ(10, hello_world.rfind('d'));
	EXPECT_EQ(9, hello_world.rfind('l'));
	EXPECT_EQ(string::npos, hello_world.rfind('e', 0));
	EXPECT_EQ(1, hello_world.rfind('e', 1));
	EXPECT_EQ(0, hello_world.rfind('h', 1));
	EXPECT_EQ(string::npos, hello_world.rfind('x'));
	EXPECT_EQ(string::npos, string().rfind('x'));

}

TEST_F(mono_string_test, finds_last_substring)
{
	EXPECT_EQ(0, hello_world.rfind("hello"));
	EXPECT_EQ(6, hello_world.rfind("world"));
	EXPECT_EQ(6, string("hello hello").rfind("hello"));
	EXPECT_EQ(string::npos, hello_world.rfind("world", 1));
	EXPECT_EQ(0, hello_world.rfind("hello", 1));
	EXPECT_EQ(0, hello_world.rfind("hello", 0));

	EXPECT_EQ(string::npos, hello_world.rfind("beautiful"));
	EXPECT_EQ(string::npos, string().rfind("world"));
}

TEST_F(mono_string_test, finds_first_character_of_set)
{
	EXPECT_EQ(0, hello_world.find_first_of("hello"));
	EXPECT_EQ(0, hello_world.find_first_of("olleh"));
	EXPECT_EQ(1, hello_world.find_first_of("hello", 1));
	EXPECT_EQ(10, hello_world.find_first_of("xyzd"));

	EXPECT_EQ(string::npos, hello_world.find_first_of("xyz"));
	EXPECT_EQ(string::npos, hello_world.find_first_of("xyz", 11));
}

TEST_F(mono_string_test, finds_first_not_matching_char_from_given_set)
{
	EXPECT_EQ(5, hello_world.find_first_not_of("hello"));
	EXPECT_EQ(5, hello_world.find_first_not_of("olleh"));
	EXPECT_EQ(6, hello_world.find_first_not_of("hello", 6));

	EXPECT_EQ(string::npos, hello_world.find_first_not_of("hello world"));
	EXPECT_EQ(string::npos, hello_world.find_first_not_of("hello world", 11));
}

TEST_F(mono_string_test, finds_first_not_matching_char)
{
	EXPECT_EQ(1, hello_world.find_first_not_of('h'));
	EXPECT_EQ(2, hello_world.find_first_not_of('h', 2));
	EXPECT_EQ(6, hello_world.find_first_not_of("hello", 6));

	EXPECT_EQ(string::npos, string("hhh").find_first_not_of('h'));
	EXPECT_EQ(string::npos, string("hhh").find_first_not_of('h', 3));
}

TEST_F(mono_string_test, finds_last_character_of_given_set)
{
	EXPECT_EQ(1, string("hh").find_last_of("h"));
	EXPECT_EQ(1, string("hh").find_last_of("abch"));
	EXPECT_EQ(0, string("hh").find_last_of("h", 0));
	EXPECT_EQ(string::npos, string("hh").find_last_of("x"));
	EXPECT_EQ(string::npos, string("hello").find_last_of("l", 1));
}

TEST_F(mono_string_test, finds_last_matching_char)
{
	EXPECT_EQ(1, string("hh").find_last_of('h'));
	EXPECT_EQ(0, string("hh").find_last_of('h', 0));
}

TEST_F(mono_string_test, finds_last_character_of_not_given_set)
{
	EXPECT_EQ(1, string("hh").find_last_not_of("x"));
	EXPECT_EQ(1, string("hh").find_last_not_of("xyz"));
	EXPECT_EQ(0, string("hh").find_last_not_of("x", 0));
	EXPECT_EQ(string::npos, string("hh").find_last_not_of("h"));
	EXPECT_EQ(string::npos, string("hh").find_last_not_of("hxyz"));
	EXPECT_EQ(string::npos, string("hello").find_last_not_of("he", 1));
}

TEST_F(mono_string_test, finds_last_not_matching_char)
{
	EXPECT_EQ(1, string("hh").find_last_not_of('x'));
	EXPECT_EQ(0, string("hh").find_last_not_of('x', 0));
	EXPECT_EQ(string::npos, string("hh").find_last_not_of('h'));
	EXPECT_EQ(string::npos, string("hello").find_last_not_of('h', 0));
}

