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

