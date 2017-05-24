#include <gtest/gtest.h>
#include "haisu/mono_hash.h"

struct mono_hash_test : public ::testing::Test
{
	haisu::mono::hash<int, int, 16, haisu::mono::collide_hash<7>> hash;

	void fill_to_capacity()
	{
		for (int i = 1; i <= hash.capacity(); ++i)
		{
			hash[i] = i;
		}
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
