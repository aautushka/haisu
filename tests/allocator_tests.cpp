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

struct allocator_test : ::testing::Test
{
	using mem_t = haisu::podbump;
	using allocator_t = haisu::allocator<int, mem_t>;

	mem_t mem;
	allocator_t allocator;

	allocator_test()
		: allocator(mem)
	{
	}
};

TEST_F(allocator_test, allocates_memory)
{
	int* p = allocator.allocate(1);
	EXPECT_TRUE(p != nullptr);
}

TEST_F(allocator_test, integrates_with_standard_containers)
{
	std::vector<int, allocator_t> vec(allocator);
	vec.push_back(0xdeadbeef);
	vec.push_back(0xbaddcafe);

	EXPECT_EQ(0xdeadbeef, vec[0]);
	EXPECT_EQ(0xbaddcafe, vec[1]);
}
