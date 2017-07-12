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

#include <type_traits>
#include <iterator>

#include "algo.h"

namespace haisu
{
namespace mono
{
template <int N>
class string
{
public:
    using reference = char&;
    using const_reference = const char&;
    using size_type = size_t;
    using value_type = char;
    using pointer = char*;
    using const_pointer = char*;

    enum constants : size_type {
        npos = std::string::npos
    };

    template <typename string_type>
    class string_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = char;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<std::is_const<string_type>::value, const char, char>&;
        using pointer = std::add_pointer<reference>; 
        using self_type = string_iterator;

        constexpr string_iterator()
        {
        }

        constexpr string_iterator(string_type* parent, difference_type cursor)
            : _parent(parent)
            , _cursor(cursor)
        {
        }

        template <typename T>
        constexpr string_iterator(const string_iterator<T>& rhs)
            : _parent(rhs._parent)
            , _cursor(rhs._cursor)
        {
        }

        constexpr reference operator *()
        {
            return _parent->at(_cursor);
        }

        constexpr pointer operator->() const
        {
            return &*this;
        }

        constexpr self_type& operator ++()
        {
            ++_cursor;
            return *this;
        }

        constexpr self_type operator ++(int)
        {
            auto ret = *this;
            ++(*this);
            return ret;
        }

        constexpr self_type& operator --()
        {
            --_cursor;
            return *this;
        }

        constexpr self_type operator --(int)
        {
            auto ret = *this;
            --(*this);
            return ret;
        }

        constexpr self_type operator +(difference_type delta) const
        {
            auto ret = *this;
            return ret += delta;
        }

        constexpr self_type& operator +=(difference_type delta)
        {
            _cursor += delta;
            return *this;
        }

        constexpr self_type operator -(difference_type delta) const
        {
            auto ret = *this;
            return ret -= delta;
        }

        constexpr self_type& operator -=(difference_type delta)
        {
            _cursor -= delta;
            return *this;
        }

        constexpr difference_type operator -(const self_type& rhs) const
        {
            return _cursor - rhs._cursor;
        }

        constexpr reference operator [](difference_type index) const
        {
            return *(*this + index);
        }

        constexpr friend bool operator ==(const self_type& lhs, const self_type& rhs)
        {
            return lhs._parent == rhs._parent && lhs._cursor == rhs._cursor;
        }

        constexpr friend bool operator !=(const self_type& lhs, const self_type& rhs)
        {
            return !(lhs == rhs);
        }

        constexpr friend bool operator <(const self_type& lhs, const self_type& rhs)
        {
            assert(lhs._parent == rhs._parent);
            return lhs._cursor < rhs._cursr;
        }

        constexpr friend bool operator <=(const self_type& lhs, const self_type& rhs)
        {
            return !(lhs > rhs);
        }

        constexpr friend bool operator >(const self_type& lhs, const self_type& rhs)
        {
            assert(lhs._parent == rhs._parent);
            return lhs._cursor > rhs._cursr;
        }

        constexpr friend bool operator >=(const self_type& lhs, const self_type& rhs)
        {
            return !(lhs < rhs);
        }

        void swap(self_type& rhs)
        {
            std::swap(_parent, rhs._parent);
            std::swap(_cursor, rhs._cursor);
        }

    private:
        string_type* _parent = nullptr;
        difference_type _cursor = 0;

        template <typename T>
        friend class string_iterator;
    };

    using iterator = string_iterator<string>;
    using const_iterator = string_iterator<const string>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin()
    {
        return iterator(this, 0);
    }

    iterator end()
    {
        return iterator(this, size());
    }

    const_iterator begin() const
    {
        return const_iterator(this, 0);
    }

    const_iterator end() const
    {
        return const_iterator(this, size());
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    /*const iterator cbegin() const
    {
        return const_iterator(this, 0);
    }

    const_iterator cend() const
    {
        return const_iterator(this, size());
    }*/

    constexpr string()
    {
        clear();
    }

    constexpr string(size_type count, char ch)
    {
        assign(count, ch);
    }

    constexpr string(const string& str)
    {
        assign(str);
    }

    template <int M>
    constexpr string(const string<M>& str)
    {
        assign(str._buf, str.size());
    }

    constexpr string& operator =(const string& str)
    {
        return assign(str);
    }

    template <int M>
    constexpr string(const string<M>& str, size_type pos, size_type count = npos)
    {
        assign(str, pos, count);
    }

    template <int M>
    constexpr string& operator =(const string<M>& str)
    {
        return assign(str._buf, str.size());
    }

    constexpr string& operator =(const char* str)
    {
        return assign(str);
    }

    constexpr string& operator =(char ch)
    {
        _buf[0] = ch;
        resize(1);
        return *this;
    }

    constexpr string(const char* str, size_type count)
    {
        assign(str, count);
    }

    constexpr string(const char* str)
    {
        assign(str);
    }

    constexpr string& assign(size_type count, char ch)
    {
        assert(count <= N);
        memset(_buf, ch, count);
        resize(count);

        return *this;
    }

    constexpr string& assign(const string& str)
    {
        memcpy(_buf, str._buf, capacity());
        return *this;
    }

    template <int M>
    constexpr string& assign(const string<M>& str)
    {
        assign(str._buf, str.size());
    }

    constexpr string& assign(const char* str, size_type count)
    {
        assert(count <= N);
        memcpy(_buf, str, count);
        resize(count);
        return *this;
    }

    template <int M>
    constexpr string& assign(const string<M>& str, size_t pos, size_t count = npos)
    {
        assert(pos < str.size());

        size_t sz = std::min(count, str.size() - pos);

        assert(sz <= N);
        assign(str._buf + pos, sz);
        
        return *this;
    }

    constexpr string& assign(const char* str)
    {
        return assign(str, strlen(str));
    }
    
    constexpr const char* c_str() const
    {
        return data();
    }
    
    constexpr const char* data() const
    {
        return _buf;
    }

    constexpr char* data() 
    {
        return _buf;
    }

    constexpr size_t size() const
    {
        return N - _buf[N];
    }

    constexpr size_t length() const
    {
        return size();
    }

    constexpr bool empty() const
    {
        return size() == 0;    
    }

    constexpr static size_t capacity()
    {
        return N;
    }

    constexpr void clear()
    {
        _buf[0] = 0;
        _buf[N] = N;
    }

    constexpr void resize(size_type count)
    {
        assert(count <= N);

        _buf[N] = N - count;
        _buf[count] = 0;
    }

    constexpr reference at(size_type pos) 
    {
        assert(pos < N);
        return _buf[pos];
    }

    constexpr const_reference at(size_t pos) const
    {
        assert(pos < N);
        return _buf[pos];
    }

    constexpr reference operator [](size_t pos)
    {
        return at(pos);
    }

    constexpr const_reference operator [](size_t pos) const
    {
        return at(pos);
    }

    constexpr reference front()
    {
        assert(size() > 0);
        return _buf[0];
    }

    constexpr const_reference front() const
    {
        assert(size() > 0);
        return _buf[0];
    }

    constexpr reference back()
    {
        assert(size() > 0);
        return _buf[size() - 1];
    }

    constexpr const_reference back() const
    {
        assert(size() > 0);
        return _buf[size() = 1];
    }

    constexpr static size_type max_size()
    {
        return capacity();
    }

    constexpr string& append(size_type count, char ch)
    {
        const size_type sz = size();

        assert(sz + count <= N);
        memset(_buf + sz, ch, count);
        resize(sz + count);
        return *this;
    }

    template <int M>
    constexpr string& append(const string<M>& str)
    {
        append(str._buf, str.size());
        return *this;
    }

    constexpr string& append(const char* str, size_type count)
    {
        const size_type sz = size();

        assert(sz + count <= N);
        memcpy(_buf + sz, str, count);
        resize(sz + count);
        return (*this);
    }

    constexpr string& append(const char* str)
    {
        return append(str, strlen(str));
    }

    constexpr size_type free_capacity() const
    {
        return capacity() - size();
    }

    constexpr string& operator +=(const string& str)
    {
        return append(str);
    }

    constexpr string& operator +=(char ch)
    {
        return append(1, ch);
    }

    constexpr void push_back(char ch)
    {
        append(1, ch);
    }

    constexpr void pop_back()
    {
        assert(!empty());
        resize(size() - 1);
    }

    template <int M>
    constexpr int compare(const string<M>& str) const
    {
        return strcmp(_buf, str._buf);
    }

    constexpr int compare(const char* str) const
    {
        return strcmp(_buf, str);
    }

    template <int M>
    constexpr bool operator ==(const string<M>& str) const
    {
        return 0 == compare(str);
    }

    template <int M>
    constexpr bool operator !=(const string<M>& str) const
    {
        return 0 != compare(str);
    }

    template <int M>
    constexpr bool operator <(const string<M>& str) const
    {
        return compare(str) < 0;
    }

    template <int M>
    constexpr bool operator >(const string<M>& str) const
    {
        return compare(str) > 0;
    }

    template <int M>
    constexpr bool operator <=(const string<M>& str) const
    {
        return compare(str) <= 0;
    }

    template <int M>
    constexpr bool operator >=(const string<M>& str) const
    {
        return compare(str) >= 0;
    }

    constexpr string& erase(size_type index = 0, size_type count = npos)
    {
        assert(index <= size());
        const size_type cur_size = size();
        const size_type erase_size = std::min(count, cur_size - index);    
        const size_type erase_end = index + erase_size;
        const size_type leftover = cur_size - erase_end;
        memmove(_buf + index, _buf + erase_end, leftover);
        resize(cur_size - erase_size);
        return *this;
    }

    constexpr string substr(size_type pos = 0, size_type count = npos) const
    {
        assert(pos <= size());
        
        return string(_buf + pos, std::min(count, size() - pos));        
    }

    size_type copy(char* dest, size_type count, size_type pos = 0) const
    {
        assert(pos <= size());

        const size_t sz = std::min(size() - pos, count);
        memcpy(dest, _buf + pos, sz); 

        return sz;
    }

    template <int M>
    size_type find(const string<M>& str, size_type pos = 0) const
    {
        return find(str._buf, pos);    
    }

    size_type find(const char* str, size_type pos, size_type count) const
    {
        assert(pos <= size());
        const void* found = memmem(_buf + pos, size() - pos, str, count);
        return found ? static_cast<const char*>(found) - _buf : npos;
    }

    size_type find(const char* str, size_type pos = 0) const
    {
        assert(pos <= size());
        const char* found = strstr(_buf + pos, str);
        return found ? found - _buf : npos;
    }

    size_type find(char ch, size_type pos = 0) const
    {
        assert(pos <= size());
        const char* found = strchr(_buf + pos, ch);
        return found ? found - _buf : npos;
    }

    string& insert(size_type index, size_type count, char ch)
    {
        assert(size() + count <= N);

        char* const rhs = _buf + index;
        memmove(rhs + count, rhs, size() - index);
        memset(rhs, ch, count);
        resize(size() + count);


        return *this;
    }

    string& insert(size_type index, const char* str)
    {
        return insert(index, str, strlen(str));
    }

    string& insert(size_type index, const char* str, size_type count)
    {
        assert(size() + count <= N);

        char* const rhs = _buf + index;
        memmove(rhs + count, rhs, size() - index);
        memcpy(rhs, str, count);
        resize(size() + count);

        return *this;
    }

    template <int M>
    string& insert(size_type index, const string<M>& str)
    {
        return insert(index, str.c_str(), str.size());
    }

    template <int M>
    string& insert(size_type index, const string<M>& str, size_type index_str, size_type count = npos)
    {
        assert(index_str <= str.size());

        const size_type max_insert_count = str.size() - index_str;
        const size_type effective_count = std::min(count, max_insert_count);

        return insert(index, str._buf + index_str, effective_count);
    }

    template <int M>
    string& replace(size_type pos, size_type count, const string<M>& str)
    {
        return replace(pos, count, str._buf, str.size());
    }

    string& replace(size_type pos, size_type count, const char* str, size_type count2)
    {
        assert(pos <= size());
        assert(pos + count <= size());
        assert(size() - count + count2 >= size());

        char* const insert_pos = _buf + pos;
        memmove(insert_pos + count2, insert_pos + count, size() - pos - count);
        memcpy(insert_pos, str, count2);

        resize(size() + count2 - count);
        
        return *this;
    }

    string& replace(size_type pos, size_type count, const char* str)
    {
        return replace(pos, count, str, strlen(str));
    }

    string& replace(size_type pos, size_type count, size_type count2, char ch)
    {
        assert(pos <= size());
        assert(pos + count <= size());
        assert(size() - count + count2 >= size());

        char* const insert_pos = _buf + pos;
        memmove(insert_pos + count2, insert_pos + count, size() - pos - count);
        memset(insert_pos, ch, count2);

        resize(size() + count2 - count);
        
        return *this;
    }

    template <int M>
    size_type rfind(const string<M>& str, size_type pos = npos) const
    {
        return rfind(str._buf, pos, str.size());
    }

    size_type rfind(const char* str, size_type pos, size_type count) const
    {
        if (count == 0)
        {
            return pos == npos ? size() : 0;
        }

        if (pos < size())
        {
            pos = pos + count;
        }

        pos = std::min(pos, size());
        const void *found = algo::memrmem(_buf, pos, str, count); 
        return found ? static_cast<const char*>(found) - _buf : npos;

    }

    size_type rfind(const char* str, size_type pos = npos) const
    {
        return rfind(str, pos, strlen(str));
    }

    size_type rfind(char ch, size_type pos = npos) const
    {
        pos = std::min(pos == npos ? pos : pos + 1, size());
        const void *found = memrchr(_buf, ch, pos); 
        return found ? static_cast<const char*>(found) - _buf : npos;
    }

    template <int M>
    size_type find_first_of(const string<M>& str, size_type pos = 0) const
    {
        return find_first_of(str._buf, pos, str.size());
    }

    size_type find_first_of(const char* str, size_type pos, size_type count) const
    {
        assert(pos <= size());
        const char* cur = _buf + pos;
        while (*cur)
        {
            if (memchr(str, *cur, count))
            {
                return cur - _buf;
            }
            ++cur;
        }

        return npos;
    }

    size_type find_first_of(const char* str, size_type pos = 0) const
    {
        return find_first_of(str, pos, strlen(str));
    }

    size_type find_first_of(char ch, size_type pos = 0) const
    {
        return find(ch, pos);
    }

    template <int M>
    size_type find_first_not_of(const string<M>& str, size_type pos = 0) const
    {
        return find_first_not_of(str._buf, pos, str.size());
    }

    size_type find_first_not_of(const char* str, size_type pos, size_type count) const
    {
        assert(pos <= size());
        const char* cur = _buf + pos;
        while (*cur)
        {
            if (!memchr(str, *cur, count))
            {
                return cur - _buf;
            }
            ++cur;
        }

        return npos;
    }

    size_type find_first_not_of(const char* str, size_type pos = 0) const
    {
        return find_first_not_of(str, pos, strlen(str));
    }

    size_type find_first_not_of(char ch, size_type pos = 0) const
    {
        assert(pos <= size());
        const char* cur = _buf + pos;
        while (*cur)
        {
            if (*cur != ch)
            {
                return cur - _buf;
            }
            ++cur;
        }

        return npos;
    }

    template <int M>
    size_type find_last_of(const string<M>& str, size_type pos = npos) const
    {
        return find_last_of(str._buf, pos, str.size());
    }

    size_type find_last_of(const char* str, size_type pos, size_type count) const
    {
        if (!empty())
        {
            pos = std::min(pos, size() - 1);
            const char* cur = _buf + pos;
            while (cur >= _buf)
            {
                auto found = static_cast<const char*>(memchr(str, *cur, count));
                if (found) { return cur - _buf; } 
                --cur;
            }
        }
        return npos;
    }

    size_type find_last_of(const char* str, size_type pos = npos) const
    {
        return find_last_of(str, pos, strlen(str));
    }

    size_type find_last_of(char ch, size_type pos = npos) const
    {
        return rfind(ch, pos);
    }

    template <int M>
    size_type find_last_not_of(const string<M>& str, size_type pos = npos) const
    {
        return find_last_not_of(str._buf, pos, str.size());
    }

    size_type find_last_not_of(const char* str, size_type pos, size_type count) const
    {
        if (!empty())
        {
            pos = std::min(pos, size() - 1);
            const char* cur = _buf + pos;
            while (cur >= _buf)
            {
                auto found = static_cast<const char*>(memchr(str, *cur, count));
                if (!found) { return cur - _buf; }
                --cur;
            }
        }
        return npos;
    }

    size_type find_last_not_of(const char* str, size_type pos = npos) const
    {
        return find_last_not_of(str, pos, strlen(str));
    }

    size_type find_last_not_of(char ch, size_type pos = npos) const
    {
        if (!empty())
        {
            pos = std::min(pos, size() - 1);
            const char* cur = _buf + pos;
            while (cur >= _buf)
            {
                if (*cur != ch) { return cur - _buf; }
                --cur;
            }
        }
        return npos;
    }
    

private:
    char _buf[N + 1];

    template <int M> friend class string;
};

static_assert(16 == sizeof(string<15>), "too big overhead");

template <int N>
std::ostream& operator <<(std::ostream& stream, const string<N>& str)
{
    return stream << str.c_str();
}

}
}
