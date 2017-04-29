
#include <gtest/gtest.h>
#include "haisu/algo.h"

struct memrmem_tests : ::testing::Test
{
	const char* memrmem(const char* haystack, const char* needle)
	{
		return (const char*)haisu::algo::memrmem(haystack, strlen(haystack), needle, strlen(needle));
	}

	const char* memrmem(const char* haystack, size_t haystackLen, const char* needle)
	{
		return (const char*)haisu::algo::memrmem(haystack, haystackLen, needle, strlen(needle));
	}
};

TEST_F(memrmem_tests, finds_substring)
{
	EXPECT_STREQ("hello world", memrmem("hello world", "hello"));
	EXPECT_STREQ("world", memrmem("hello world", "world"));
}

TEST_F(memrmem_tests, finds_last_occurence)
{
	EXPECT_STREQ("hello", memrmem("hello hello", "hello"));
}

TEST_F(memrmem_tests, cant_find_matching_substring)
{
	EXPECT_EQ(nullptr, memrmem("hello world", "beaufiful"));
}

TEST_F(memrmem_tests, does_not_read_beyond_the_buffer)
{
	EXPECT_NE(nullptr, memrmem("hello world", 11,  "world"));
	EXPECT_EQ(nullptr, memrmem("hello world", 10,  "world"));
	EXPECT_EQ(nullptr, memrmem("hello world", 9,  "world"));
	EXPECT_EQ(nullptr, memrmem("hello world", 8,  "world"));
	EXPECT_EQ(nullptr, memrmem("hello world", 7,  "world"));
	EXPECT_EQ(nullptr, memrmem("hello world", 6,  "world"));
}

