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

#include "haisu/zbuf.h"

struct zbuf_test: ::testing::Test
{
	haisu::zbuf z;
};

TEST_F(zbuf_test, reads_int_from_zbuf)
{
	z.append(int{1});
	EXPECT_EQ(1, z.at<int>(0));
}

TEST_F(zbuf_test, appends_multiple_ints)
{
	z.append(int{1});
	z.append(int{2});
	z.append(int{3});
	
	EXPECT_EQ(1, z.at<int>(0));
	EXPECT_EQ(2, z.at<int>(1));
	EXPECT_EQ(3, z.at<int>(2));
}

TEST_F(zbuf_test, accesses_int_as_a_sequence_of_bytes)
{
	z.append(int{0x01020304});
	
	EXPECT_EQ(4, z.at<char>(0));
	EXPECT_EQ(3, z.at<char>(1));
	EXPECT_EQ(2, z.at<char>(2));
	EXPECT_EQ(1, z.at<char>(3));
}

TEST_F(zbuf_test, modifies_zbuf_member)
{
	z.append(int{1});
	z.append(int{2});

	z.at<int>(1) = 3;

	EXPECT_EQ(3, z.at<int>(1));
}

TEST_F(zbuf_test, inserts_byte_in_the_middle_of_zbuf)
{
	z.append(short{0x0301});
	z.insert(1, char{0x02});
	
	EXPECT_EQ(1, z.at<char>(0));
	EXPECT_EQ(2, z.at<char>(1));
	EXPECT_EQ(3, z.at<char>(2));
}

TEST_F(zbuf_test, inserts_multibyte_in_the_middle_of_zbuf)
{
	z.append(int{0x06050201});
	z.insert(1, short{0x0403});
	
	EXPECT_EQ(1, z.at<char>(0));
	EXPECT_EQ(2, z.at<char>(1));
	EXPECT_EQ(3, z.at<char>(2));
	EXPECT_EQ(4, z.at<char>(3));
	EXPECT_EQ(5, z.at<char>(4));
	EXPECT_EQ(6, z.at<char>(5));
}

TEST_F(zbuf_test, appends_buffer_using_insert_call)
{
	z.append(short{0x01});
	z.insert(1, char{0x02});
	
	EXPECT_EQ(1, z.at<char>(0));
	EXPECT_EQ(2, z.at<char>(1));
}

TEST_F(zbuf_test, prepends_buffer_using_insert_call)
{
	z.append(short{0x02});
	z.insert(0, char{0x01});
	
	EXPECT_EQ(1, z.at<char>(0));
	EXPECT_EQ(2, z.at<char>(1));
}

TEST_F(zbuf_test, returns_buffer_size_in_bytes)
{
	z.append(int{1});
	EXPECT_EQ(sizeof(int), z.size());
}

TEST_F(zbuf_test, clears_buffer)
{
	z.append(int{1});
	z.clear();

	EXPECT_EQ(0, z.size());
}

TEST_F(zbuf_test, erases_int_from_head)
{
	z.append(int{1});
	z.append(int{2});
	
	z.erase<int>(0);

	EXPECT_EQ(2, z.at<int>(0));
}

TEST_F(zbuf_test, erases_int_from_tail)
{
	z.append(int{1});
	z.append(int{2});
	
	z.erase<int>(1);

	EXPECT_EQ(1, z.at<int>(0));
}

TEST_F(zbuf_test, erases_int_from_middle)
{
	z.append(int{1});
	z.append(int{2});
	z.append(int{3});
	
	z.erase<int>(1);

	EXPECT_EQ(3, z.at<int>(1));
}

TEST_F(zbuf_test, resizes_buffer_when_doing_erase)
{
	z.append(char{1});
	z.append(char{2});
	
	z.erase<char>(0);

	EXPECT_EQ(1u, z.size());
}

TEST_F(zbuf_test, appends_string)
{
	z.append("hello");

	EXPECT_STREQ("hello", z.ptr(0));
}

TEST_F(zbuf_test, appends_multiple_strings)
{
	z.append("hello");
	z.append("world");

	EXPECT_STREQ("hello", z.ptr(0));
	EXPECT_STREQ("world", z.ptr(6));
}

TEST_F(zbuf_test, erases_string_from_head)
{
	z.append("hello");
	z.append("world");

	z.erase<const char*>(0);

	EXPECT_STREQ("world", z.ptr(0));
}

TEST_F(zbuf_test, erases_string_from_middle)
{
	z.append("hello");
	z.append("beautiful");
	z.append("world");

	z.erase<const char*>(6);

	EXPECT_STREQ("hello", z.ptr(0));
	EXPECT_STREQ("world", z.ptr(6));
}

TEST_F(zbuf_test, erases_string_from_tail)
{
	z.append("hello");
	z.append("world");

	z.erase<const char*>(6);

	EXPECT_STREQ("hello", z.ptr(0));
}

TEST_F(zbuf_test, resizes_buffer_when_erasing_string)
{
	z.append("hello");
	z.append("world");

	z.erase<const char*>(6);

	EXPECT_EQ(6u, z.size());
}

TEST_F(zbuf_test, inserts_string_in_the_head)
{
	z.insert(0, "hello");

	EXPECT_STREQ("hello", z.ptr(0));
}

TEST_F(zbuf_test, inserts_string_in_the_head_twice)
{
	z.insert(0, "world");
	z.insert(0, "hello");

	EXPECT_STREQ("hello", z.ptr(0));
	EXPECT_STREQ("world", z.ptr(6));
}

TEST_F(zbuf_test, inserts_string_in_the_middle)
{
	z.insert(0, "world");
	z.insert(0, "hello");
	z.insert(6, "new");

	EXPECT_STREQ("hello", z.ptr(0));
	EXPECT_STREQ("new", z.ptr(6));
	EXPECT_STREQ("world", z.ptr(10));
}

