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
#include "haisu/mono_hash.h"

struct mono_hash_test : public ::testing::Test
{
    using no_throw_hash = haisu::mono::hash<int, int, 16, haisu::mono::collide_hash<7>>;
    using throw_hash = haisu::mono::hash<int, int, 16, haisu::mono::collide_hash<7>, haisu::mono::do_throw<std::exception>>;

    no_throw_hash hash;

    template <typename Hash>
    void fill_to_capacity(Hash& hash)
    {
        for (int i = 1; i <= hash.capacity(); ++i)
        {
            hash[i] = i;
        }
    }

    void fill_to_capacity()
    {
        fill_to_capacity(hash);
    }
};

TEST_F(mono_hash_test, cant_find_non_existant_key)
{
    EXPECT_FALSE(hash.contains(5));
}

TEST_F(mono_hash_test, contains_previously_added_key)
{
    hash[5] = 1;
    EXPECT_TRUE(hash.contains(5));
}

TEST_F(mono_hash_test, reads_added_value)
{
    hash[5] = 1;
    EXPECT_EQ(1, hash[5]);
}

TEST_F(mono_hash_test, adds_several_items)
{
    hash[5] = 1;
    hash[6] = 2;
    EXPECT_EQ(1, hash[5]);
    EXPECT_EQ(2, hash[6]);
}

TEST_F(mono_hash_test, returns_cache_capacity)
{
    EXPECT_EQ(16, hash.capacity());
}


TEST_F(mono_hash_test, fills_cache_to_capacity)
{
    haisu::mono::hash<int, int, 5, haisu::mono::collide_hash<2>> hash;

    for (int i = 1; i <= hash.capacity(); ++i)
    {
        hash[i] = i;
    }

    for (int i = 1; i <= hash.capacity(); ++i)
    {
        EXPECT_TRUE(hash.contains(i));
        EXPECT_EQ(i, hash[i]);
    }
}

TEST_F(mono_hash_test, hash_is_empty)
{
    EXPECT_TRUE(hash.empty());
    EXPECT_EQ(0, hash.size());
}

TEST_F(mono_hash_test, hash_is_no_longer_empty)
{
    hash[1] = 2;
    EXPECT_FALSE(hash.empty());
    EXPECT_EQ(1, hash.size());
}

TEST_F(mono_hash_test, exhausts_cache_capacity)
{
    fill_to_capacity();
    EXPECT_FALSE(hash.empty());
    EXPECT_EQ(hash.capacity(), hash.size());
}

TEST_F(mono_hash_test, clears_hash)
{
    hash[1] = 2;
    hash.clear();

    EXPECT_TRUE(hash.empty());
}

TEST_F(mono_hash_test, throws_if_full)
{
    throw_hash hash;
    fill_to_capacity(hash);

    EXPECT_THROW((hash[123] = 456), std::exception);
}

TEST_F(mono_hash_test, throw_is_no_such_element)
{
    throw_hash hash;
    const throw_hash& chash = hash;

    EXPECT_THROW(chash[123], std::exception);
}

TEST_F(mono_hash_test, inserts_new_element)
{
    hash.insert(1, 2);
    EXPECT_EQ(2, hash[1]);
}

TEST_F(mono_hash_test, inserts_multiple_elements)
{
    hash.insert(1, 2);
    hash.insert(2, 3);
    EXPECT_EQ(2, hash[1]);
    EXPECT_EQ(3, hash[2]);
}

TEST_F(mono_hash_test, throws_if_attempting_to_insert_same_element_twice)
{
    throw_hash hash;

    hash.insert(1, 2);
    EXPECT_THROW(hash.insert(1, 3), std::exception);
}

TEST_F(mono_hash_test, throws_if_inserting_into_full_hash)
{
    throw_hash hash;
    fill_to_capacity(hash);

    EXPECT_THROW(hash.insert(888, 999), std::exception);
}

TEST_F(mono_hash_test, keeps_silent_when_trying_to_erase_nonexistant_item)
{
    hash.erase(1);
}

TEST_F(mono_hash_test, erases_item)
{
    hash.insert(1, 2);
    hash.erase(1);

    EXPECT_FALSE(hash.contains(1));
}

TEST_F(mono_hash_test, reuses_erased_capacity)
{
    fill_to_capacity(hash);
    hash.erase(1);

    // make sure we removed an item
    ASSERT_EQ(hash.size() + 1, hash.capacity());

    hash.insert(1, 1);
    EXPECT_EQ(hash.size(), hash.capacity());
}
