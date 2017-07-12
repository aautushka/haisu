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
#include "haisu/memory.h"

struct refbump_test : ::testing::Test
{
	haisu::refbump memory;
};

TEST_F(refbump_test, allocates_memory)
{
	int* p = memory.alloc<int>();
	EXPECT_TRUE(p != nullptr);
}

TEST_F(refbump_test, new_allocation_goes_to_different_address)
{
	int* p1 = memory.alloc<int>();
	int* p2 = memory.alloc<int>();

	EXPECT_TRUE(p1 != p2);
}

TEST_F(refbump_test, subsequent_allocation_do_not_interfere_with_one_another)
{
	int* p1 = memory.alloc<int>();
	int* p2 = memory.alloc<int>();

	*p1 = 0xdeadbeef;
	*p2 = 0xbaddcafe;

	EXPECT_EQ(0xdeadbeef, *p1);
	EXPECT_EQ(0xbaddcafe, *p2);
}

TEST_F(refbump_test, automatically_adds_more_memory_if_needed)
{
	for (int i = 0; i < 100000; ++i)
	{
		void* p = memory.alloc(32);
		EXPECT_TRUE(nullptr != p);
	}
}

TEST_F(refbump_test, allocates_huge_buffer_all_at_once)
{
	void* p = memory.alloc(100 * 1024 * 1024);
	EXPECT_TRUE(nullptr != p);
}

TEST_F(refbump_test, allocates_memory_of_any_imaginable_size)
{
	for (int i = 1; i < 128 * 1024 * 1024; i *= 2)
	{
		haisu::refbump memory;
		void* p = memory.alloc(i);
		EXPECT_TRUE(nullptr != p);
	}
}

TEST_F(refbump_test, allocates_one_arena)
{
	int* p = memory.alloc<int>();

	EXPECT_EQ(1, memory.arena_count());
}

TEST_F(refbump_test, allocates_two_arenas)
{
	void* p1 = memory.alloc<int>();
	void* p2 = memory.alloc(10 * 1024 * 1024); // this guarantees we have an additional huge arena

	EXPECT_EQ(2, memory.arena_count());
}

TEST_F(refbump_test, frees_large_arena_if_there_are_no_more_references)
{
	return;
	void* p1 = memory.alloc<int>();
	void* p2 = memory.alloc(10 * 1024 * 1024); // this guarantees we have an additional huge arena

	memory.free(p2);

	EXPECT_EQ(1, memory.arena_count());
}

TEST_F(refbump_test, frees_small_arena_if_there_are_no_more_references)
{
	void* p1 = memory.alloc<int>();
	void* p2 = memory.alloc(10 * 1024 * 1024); // this guarantees we have an additional huge arena

	memory.free(p1);

	EXPECT_EQ(1, memory.arena_count());
}

