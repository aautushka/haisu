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
#include "haisu/function_ref.h"

struct function_ref_test : ::testing::Test
{
};

template <typename T>
using function_ref = haisu::function_ref<T>;

TEST_F(function_ref_test, binds_to_lambda_casts_lambda_to_function_pointer)
{
    bool called = false;
    function_ref<void(bool&)> f([](bool& called) -> void{called = true;});
    
    f(called);
    EXPECT_TRUE(called);
    EXPECT_TRUE(f.safe_to_store());
}

static void global_function(bool& called)
{
    called = true;
}

static int global_function_2()
{
    return 123;
}

TEST_F(function_ref_test, binds_global_function)
{
    bool called = false;
    function_ref<void(bool&)> f(global_function);
    
    f(called);
    EXPECT_TRUE(called);
    EXPECT_TRUE(f.safe_to_store());
}

TEST_F(function_ref_test, default_constructed_function_is_not_callable)
{
    function_ref<void(void)> f;
    EXPECT_TRUE(!f);
}

TEST_F(function_ref_test, constructs_from_object)
{
    struct function_object 
    {
        int operator ()() { return 123; };
    } obj;

    function_ref<int(void)> f(obj);

    EXPECT_EQ(123, f());
    EXPECT_FALSE(f.safe_to_store());
}

TEST_F(function_ref_test, constructs_from_temp_object)
{
    struct function_object 
    {
        int operator ()() { return 123; };
    };

    function_ref<int(void)> f(function_object{});

    EXPECT_EQ(123, f());
    EXPECT_FALSE(f.safe_to_store());
    
}

TEST_F(function_ref_test, binds_to_stored_lambda_still_casts_to_function)
{
    auto lambda = []() { return 123; };
    function_ref<int()> f{lambda};
    
    EXPECT_EQ(123, f());
    EXPECT_TRUE(f.safe_to_store());
}

TEST_F(function_ref_test, binds_to_labmda_with_capture_indicating_that_its_not_safe_to_store_it)
{
    auto lambda = [&]() { return 123; };
    function_ref<int()> f{lambda};
    
    EXPECT_EQ(123, f());
    EXPECT_FALSE(f.safe_to_store());
}

TEST_F(function_ref_test, assignes_function)
{
    function_ref<int()> f;
    f = [](){return 123;};

    EXPECT_EQ(123, f());
}

TEST_F(function_ref_test, copy_constructs_function_ref)
{
    function_ref<int()> f{[]{return 123;}};
    function_ref<int()> other = f; 

    EXPECT_EQ(123, other());
}

TEST_F(function_ref_test, assignes_nullptr)
{
    function_ref<int()> f{[]{return 123;}};
    f = nullptr;

    EXPECT_FALSE(f);
}

TEST_F(function_ref_test, swaps_functions)
{
    function_ref<int()> f{[]{return 123;}};
    function_ref<int()> other;
    f.swap(other);

    EXPECT_FALSE(f);
    EXPECT_TRUE(other);

    EXPECT_EQ(123, other());
}

TEST_F(function_ref_test, compares_valid_function_to_nullptr)
{
    function_ref<int()> f{[]{return 123;}};
    EXPECT_FALSE(f == nullptr);
    EXPECT_FALSE(nullptr == f);
    EXPECT_TRUE(f != nullptr);
    EXPECT_TRUE(nullptr != f);
}

TEST_F(function_ref_test, compares_invalid_funtion_to_nullptr)
{
    function_ref<int()> f;
    EXPECT_TRUE(f == nullptr);
    EXPECT_TRUE(nullptr == f);
    EXPECT_FALSE(f != nullptr);
    EXPECT_FALSE(nullptr != f);
}

TEST_F(function_ref_test, swaps_with_std_function)
{
    function_ref<int()> f{[]{return 123;}};
    function_ref<int()> other;
    std::swap(f, other);

    EXPECT_TRUE(other);
}

TEST_F(function_ref_test, stores_lambda_with_not_matching_signature)
{
    function_ref<int(int)> f;
    f = +[](long){return 123;};
    
    EXPECT_EQ(123, f(0));
    EXPECT_TRUE(f.safe_to_store());
}

TEST_F(function_ref_test, deduces_lambda_argument_type_and_stores_function_pointer)
{
    function_ref<int(int)> f;
    f = [](auto){return 123;};
    
    EXPECT_EQ(123, f(0));
    EXPECT_TRUE(f.safe_to_store());
}

TEST_F(function_ref_test, throws_if_called_invalid_function)
{
    function_ref<int()> f;
    EXPECT_THROW(f(), std::bad_function_call);
}

