#include <gtest/gtest.h>

#include "haisu/tls.h"

struct linear_hash_test : ::testing::Test
{
	using hash_t = haisu::mono::linear_hash<int, int, 3>;
	hash_t hash;
};

TEST_F(linear_hash_test, finds_inserved_value)
{
	hash[123] = 456;
	EXPECT_EQ(456, hash[123]);
}

TEST_F(linear_hash_test, fill_the_hash)
{
	hash[1] = 123;
	hash[2] = 456;
	hash[3] = 789;

	EXPECT_EQ(123, hash[1]);
	EXPECT_EQ(456, hash[2]);
	EXPECT_EQ(789, hash[3]);
}

TEST_F(linear_hash_test, inserts_key_value_pair)
{
	hash.insert(123, 456);

	EXPECT_EQ(456, hash[123]);
}

TEST_F(linear_hash_test, replaces_hash_item_by_explicitly_erasing_it)
{
	hash[1] = 11;
	hash[2] = 22;
	hash[3] = 33;

	hash.erase(1);
	hash.insert(1, 44);

	EXPECT_EQ(44, hash[1]);
}

TEST_F(linear_hash_test, ctor_creates_empty_list)
{
	EXPECT_EQ(0, hash.size());
	EXPECT_TRUE(hash.empty());
}

TEST_F(linear_hash_test, adding_an_item_makes_hash_non_empty)
{
	hash[1] = 2;
	EXPECT_FALSE(hash.empty());
}

TEST_F(linear_hash_test, changes_hash_size)
{
	hash[1] = 2;
	hash[2] = 3;
	EXPECT_EQ(2, hash.size());
}

TEST_F(linear_hash_test, call_to_insert_changes_hash_size)
{
	hash.insert(1, 2);
	EXPECT_EQ(1, hash.size());
}

TEST_F(linear_hash_test, changing_hash_size_by_erasing_an_element)
{
	hash.insert(1, 2);
	hash.erase(1);
	EXPECT_EQ(0, hash.size());
}

TEST_F(linear_hash_test, subsequent_reads_do_not_change_the_hash_size)
{
	hash[1] = 2;
	hash[1] = 3;
	EXPECT_EQ(1, hash.size());
}

TEST_F(linear_hash_test, reassignes_hash_value)
{
	hash[1] = 2;
	hash[1] = 3;
	EXPECT_EQ(3, hash[1]);
}

TEST_F(linear_hash_test, clears_hash)
{
	hash[1] = 2;
	hash[2] = 1;
	
	hash.clear();

	EXPECT_EQ(0, hash.size());
}

TEST_F(linear_hash_test, traverses_assigned_nodes)
{
	hash[1] = 2;
	hash[3] = 4;

	std::map<int, int> actual;
	std::map<int, int> expected = {{1, 2}, {3, 4}};

	hash.foreach([&](int key, int val){actual[key] = val; });

	EXPECT_EQ(expected, actual);
}
