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

#include <vector>
#include <cstring>

namespace haisu
{

class zbuf
{
public:
    template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>{}>> 
    void append(T t)
    {
        const size_t prev = buf.size();
        const size_t sz = sizeof(t);
        buf.resize(prev + sz);
        memcpy(&buf[prev], &t, sz);
    }

    template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>{}>> 
    T& append()
    {
        const size_t prev = buf.size();
        const size_t sz = sizeof(T);
        buf.resize(prev + sz);
        return *reinterpret_cast<T*>(&buf[prev]);
    }

    template <typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>{}>> 
    T& append_modify(T t)
    {
        const size_t prev = buf.size();
        const size_t sz = sizeof(t);
        buf.resize(prev + sz);
        memcpy(&buf[prev], &t, sz);
        return *reinterpret_cast<T*>(&buf[prev]);
    }

    void append(const void* ptr, size_t len)
    {
        const size_t prev = buf.size();
        buf.resize(prev + len);
        memcpy(&buf[prev], &ptr, len);
    }

    template <typename T>
    const T& at(size_t index) const noexcept
    {
        return at_offset<T>(index * sizeof(T));
    }

    template <typename T>
    T& at(size_t index) noexcept
    {
        return at_offset<T>(index * sizeof(T));
    }

    template <typename T>
    const T& at_offset(size_t offset) const noexcept
    {
        assert(offset < size() && size() - offset >= sizeof(T));
        return *reinterpret_cast<const T*>(&buf[offset]);
    }

    template <typename T>
    T& at_offset(size_t offset) noexcept
    {
        assert(offset < size() && size() - offset >= sizeof(T));
        return *reinterpret_cast<T*>(&buf[offset]);
    }

    template <typename T>
    void insert(size_t index, T t)
    {
        const size_t prev = buf.size();
        const size_t sz = sizeof(t);
        buf.resize(prev + sz);
        
        auto source = &buf[index * sz];
        auto dest = source + sz;
        memmove(dest, source, prev - index * sz);
        at<T>(index) = t;
    }

    template <typename T>
    void erase(size_t index)
    {
        const auto dest = index * sizeof(T);
        const auto src = dest + sizeof(T);
        const auto size = buf.size() - src;
        
        auto destptr = &buf[dest];
        auto sourceptr = destptr + sizeof(T);

        memmove(destptr, sourceptr, size);
        buf.resize(buf.size() - sizeof(T));
    }

    size_t size() const noexcept
    {
        return buf.size();
    }

    bool empty() const noexcept
    {
        return buf.empty();
    }

    size_t capacity() const noexcept
    {
        return buf.capacity();
    }

    const char* ptr(size_t index) const
    {
        return &buf[index];
    }

    void reserve(size_t bytes)
    {
        buf.reserve(bytes);
    }

    void grow(size_t delta)
    {
        reserve(buf.size() + delta);
    }

    void clear() noexcept
    {
        buf.clear();
    }

private:
    std::vector<char> buf;
};

template <> inline
void zbuf::append(const char* t)
{
    const size_t prev = buf.size();
    const size_t sz = strlen(t) + 1;
    buf.resize(prev + sz);
    memcpy(&buf[prev], t, sz);
}

template <> inline
void zbuf::erase<const char*>(size_t index)
{
    const size_t sz = strlen(&buf[index]) + 1;
    const size_t from = index + sz;
    
    auto dest = &buf[index];
    auto source = dest + sz;
    memmove(dest, source, buf.size() - from);
    buf.resize(buf.size() - sz);
}

template <> inline
void zbuf::insert<const char*>(size_t index, const char* t)
{
    const size_t prev = buf.size();
    const size_t sz = strlen(t) + 1;
    buf.resize(prev + sz);
        
    auto source = &buf[index];
    auto dest = source + sz;
    memmove(dest, source, prev - index);
    memcpy(source, t, sz);
}

}

