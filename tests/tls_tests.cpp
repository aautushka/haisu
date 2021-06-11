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

