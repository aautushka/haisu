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

