
#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "haisu/tls.h"

struct tls_test : ::testing::Test
{
	enum {N = 3};
	using tls_t = haisu::tls<int, N>;
	tls_t tls;
};

TEST_F(tls_test, returns_null_pointer_if_not_yet_assigned)
{
	EXPECT_EQ(nullptr, tls.get());
}

TEST_F(tls_test, reads_assigned_value)
{
	tls.reset(new int(123));
	EXPECT_EQ(123, *tls);
}

TEST_F(tls_test, reassignes_value_and_reads_it)
{
	tls.reset(new int(123));
	tls.reset(new int(456));

	EXPECT_EQ(456, *tls);
}

TEST_F(tls_test, returns_null_pointer_after_reset)
{
	tls.reset(new int(123));
	tls.reset();

	EXPECT_EQ(nullptr, tls.get());
}

TEST_F(tls_test, returns_null_pointer_after_release)
{
	tls.reset(new int(123));
	auto ptr = std::unique_ptr<int>(tls.release());

	EXPECT_EQ(nullptr, tls.get());
}

TEST_F(tls_test, releases_previously_assigned_pointer)
{
	tls.reset(new int(123));
	auto ptr = std::unique_ptr<int>(tls.release());

	EXPECT_EQ(123, *ptr);
}

TEST_F(tls_test, clears_this_thread_data)
{
	tls.reset(new int(123));
	tls.clear();

	EXPECT_EQ(nullptr, tls.get());
}

TEST_F(tls_test, other_thread_has_its_own_separate_storage)
{
	tls.reset(new int(123));

	std::thread other([&]{tls.reset(new int(456));});
	other.join();

	EXPECT_EQ(123, *tls);
}

TEST_F(tls_test, clears_data_in_all_threads)
{
	//rely on valgrind to find possible memory leaks
	tls.reset(new int(123));

	std::thread other([&]{tls.reset(new int(456));});
	other.join();

	tls.clear();

	EXPECT_EQ(nullptr, tls.get());
}

TEST_F(tls_test, reuses_the_cleared_data)
{
	haisu::tls<int, 1> tls;

	std::thread other([&]{tls.reset(new int(456));});
	other.join();

	tls.clear();
	tls.reset(new int(123));

	EXPECT_EQ(123, *tls);
}

