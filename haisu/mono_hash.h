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

For more information, please refer to <http://unlicense.org/>
*/

#pragma once
#include <type_traits>
#include <cassert>
#include "meta.h"

namespace haisu
{
namespace mono
{

template <int N>
struct collide_hash
{
    template <typename T> int operator() (const T&) const { return N; }
};

struct do_assert
{
    void operator ()(const char* message) { assert(message && false);}
};

template <typename Exception>
struct do_throw
{
    void operator ()(const char* message) { throw Exception(); }
};

struct do_nothing
{
    void operator ()(const char*) { }
};

template <typename T>
struct direct_hash
{
    constexpr T operator ()(T t) const { return t; }
};

template <typename T>
struct amiga_hash
{
    constexpr T operator ()(T t) const
    {
        return t * 0xdeece66d + 0xb;
    }
};


struct hash_traits
{
    void signal_error()
    {
        
    }

    void probe(int& index)
    {
    }
};

template <typename Key, typename Val, int N, typename Hash = std::hash<Key>, typename Throw = do_assert>
class hash
{
public:
    using size_type = meta::memory_requirement_t<N>;
    using key_type = Key;
    using value_type = Val;
    using hash_type = Hash;

    hash()
    {
        clear();
    }

    value_type& operator [](key_type key)
    {
        return at(key);
    }

    const value_type& operator [](key_type key) const
    {
        return at(key);
    }
    
    bool contains(key_type key) const
    {
        return -1 != find_index(key);
    }

    value_type& at(key_type key)
    {
        const auto index = get_possible_index(key);
        auto cur = index;

        do 
        {
            const auto k = get_key(cur);
            if (k == key)
            {
                return get_value(cur);
            }
            else if (k == 0)
            {
                table_[cur].first = key;
                return get_value(cur);
            }
            probe(cur);
        }
        while (cur < N);

        cur = 0;
        while (cur < index)
        {
            const auto k = get_key(cur);
            if (k == key)
            {
                return get_value(cur);
            }
            else if (k == 0)
            {
                table_[cur].first = key;
                return get_value(cur);
            }

            probe(cur);
        }

        signal_error("hash is full");
        abort();
    }

    void insert(key_type key, value_type val)
    {
        const auto index = get_possible_index(key);
        auto cur = index;

        do 
        {
            const auto k = get_key(cur);
            if (k == 0)
            {
                table_[cur].first = key;
                table_[cur].second = val;
                return;
            }
            else if (k == key)
            {
                signal_error("duplicate entry");
            }
            probe(cur);
        }
        while (cur < N);

        cur = 0;
        while (cur < index)
        {
            const auto k = get_key(cur);
            if (k == 0)
            {
                table_[cur].first = key;
                table_[cur].second = val;
                return;
            }
            else if (k == key)
            {
                signal_error("duplicate entry");
            }
            probe(cur);
        }

        signal_error("hash is full");
    }

    const value_type& at(key_type key) const
    {
        const auto index = get_possible_index(key);
        auto cur = index;

        do 
        {
            const auto k = get_key(cur);
            if (k == key)
            {
                return get_value(cur);
            }
            else if (k == 0)
            {
                signal_error("there is no such key in the hash");
            }
            probe(cur);
        }
        while (cur < N);

        cur = 0;
        while (cur < index)
        {
            const auto k = get_key(cur);
            if (k == key)
            {
                return get_value(cur);
            }
            else if (k == 0)
            {
                signal_error("there is no suck key in the hash");
            }
            probe(cur);
        }

        signal_error("hash is full");
        abort();
    }

    bool empty() const
    {
        return 0 == size();
    }

    size_type size() const
    {
        auto ret = size_type{0};
        for (int i = 0; i < N; ++i)
        {
            if (table_[i].first != 0)
            {
                ++ret;
            }
        }
        return ret;
    }

    void clear()
    {
        clear(std::is_integral<Key>());
    }

    constexpr size_type capacity() const
    {
        return N;
    }

    void erase(key_type key)
    {
        const auto index = find_index(key);
        if (index != -1)
        {
            table_[index].first = 0;
        }
    }

private:
    void clear(std::true_type)
    {
        for (int i = 0; i < N; ++i)
        {
            table_[i].first = 0;
        }
    }

    key_type get_key(int index) const
    {
        return table_[index].first;
    }

    const value_type& get_value(int index) const
    {
        return table_[index].second;
    }

    value_type& get_value(int index)
    {
        return table_[index].second;
    }

    int get_possible_index(key_type key) const
    {
        return hash_type()(key) % N;
    }

    void signal_error(const char* message) const
    {
        Throw()(message);
    }

    int find_index(key_type key) const
    {
        const auto index = get_possible_index(key);
        auto cur = index;

        do 
        {
            const auto k = get_key(cur);
            if (k == key)
            {
                return cur;
            }
            else if (k == 0)
            {
                return -1;
            }
            ++cur;
        }
        while (cur < N);

        cur = 0;
        while (cur < index)
        {
            const auto k = get_key(cur);
            if (k == key)
            {
                return cur;
            }
            else if (k == 0)
            {
                return -1;
            }
            ++cur;
        }

        return -1;
    }

    void probe(int& index) const
    {
        ++index;
    }

    using pair_type = std::pair<key_type, value_type>;

    pair_type table_[N];
};

} // namespace mono
} // namespace haisu

