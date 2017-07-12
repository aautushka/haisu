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

#pragma once

#include <limits>
#include <vector>
#include <cstring>

#ifndef __linux__
inline const void* memrchr(const void* buf, int ch, size_t count)
{
    const uint8_t* beg = static_cast<const uint8_t*>(buf);
    const uint8_t* cur = beg + count - 1;
    while (cur >= beg) { if (*cur == ch) { return cur; } --cur; }    
    return nullptr;
}
#endif

namespace haisu
{
namespace algo
{

static const size_t npos = std::numeric_limits<size_t>::max();

template <typename T>
struct three_way_cmp
{
    int operator ()(T lhs, T rhs) const
    {
        if (lhs < rhs)
        {
            return -1;
        }
        else if (lhs > rhs)
        {
            return 1;
        }

        return 0;
    }
};

template <>
struct three_way_cmp<const char*>
{
    int operator ()(const char* lhs, const char* rhs) const
    {
        return strcmp(lhs, rhs);
    }
};

template <>
struct three_way_cmp<std::string>
{
    int operator ()(const std::string& lhs, const std::string& rhs) const
    {
        return lhs.compare(rhs);
    }
};

template <typename Container, typename Key, typename Cmp = three_way_cmp<Key>>
size_t binary_search(const Container& container, Key key, Cmp cmp = Cmp())
{
    if (container.empty()) return npos;

    int head = 0;
    int tail = container.size() - 1;

    while (head <= tail)
    {
        int mid = (head + tail) / 2;
        const int res = cmp(key, container.at(mid));
        if (res > 0)
        {
            head = mid + 1;
        }
        else if (res < 0)
        {
            tail = mid - 1;
        }
        else
        {
            return mid;
        }
    }
        
    return npos;
}


inline const void* memrmem(const void* haystack, size_t haystackLen, const void* needle, size_t needleLen)
{
    auto h = static_cast<const uint8_t*>(haystack);
    auto n = static_cast<const uint8_t*>(needle);
    auto end = h + haystackLen - needleLen + 1;
    while (h < end)
    {
        auto match = static_cast<const uint8_t*>(memrchr(h, n[0], end - h));
        if (match)
        {
            if (!memcmp(match, needle, needleLen)) { return match; }
            end = match - 1;
            continue;
        }
        break;
    }
    return nullptr;
}

}
}
