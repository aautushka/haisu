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

#include "meta.h"

namespace haisu
{
namespace mono
{

// TODO:
//  1. call descructors on clear()
//  2. do not default-construct objects
//  3. explicitly desctory objects in dtor
template <typename T, int N, typename Offset = meta::memory_requirement_t<N>>
class xorlist
{
    using offset_type = Offset;
    enum {nil = std::numeric_limits<offset_type>::max()};
    static_assert(std::numeric_limits<offset_type>::max() >= N, "");
public:
    using size_type = offset_type;
    
    xorlist()
    {
        clear_init();
    }

    xorlist(const xorlist&) = delete;
    xorlist& operator =(const xorlist&) = delete;
    xorlist(const xorlist&&) = delete;
    xorlist& operator =(const xorlist&&) = delete;

    xorlist(std::initializer_list<T> ll)
    {
        *this = std::move(ll);
    }

    xorlist& operator =(std::initializer_list<T> ll)
    {

        for (auto l: ll)
        {
            push_back(std::move(l));
        }

        return *this;
    }

    void clear()
    {
        if (_head != nil)
        {
            tail().link = _free_list;
            _free_list = _head;
            _head = _tail = nil;
        }    
    }

    void push_back(T t)
    {
        assert(!full());

        auto node = alloc();
        _buf[node].t = std::move(t);
        
        if (empty())
        {
            init(node);
        }
        else
        {
            _buf[node].link = _tail;
            tail().link ^= node;
            _tail = node;
        }
    }

    void push_front(T t)
    {
        assert(!full());
        auto node = alloc();
        _buf[node].t = std::move(t);

        if (empty())
        {
            init(node);
        }
        else
        {
            _buf[node].link = _head;
            head().link ^= node;
            _head = node;
        }
    }

    T& back()
    {
        assert(!empty());
        return tail().t;
    }

    const T& back() const
    {
        assert(!empty());
        return tail().t;
    }

    T& front()
    {
        assert(!empty());
        return head().t;
    }

    const T& front() const
    {
        assert(!empty());
        return front().t;
    }

    bool empty() const
    {
        return (_head == nil);
    }

    bool full() const
    {
        return _free_list == nil;
    }

    size_type size() const
    {
        size_type res = 0;
        offset_type cur = _head;
        offset_type prev = 0;
        offset_type next = cur;
        while (cur != _tail)
        {
            res += 1;
            next = at(cur).link ^ prev;
            prev = cur;
            cur = next;
        }    
        
        res += (_head == nil ? 0 : 1);
        return res;
    }

    size_type capacity() const
    {
        return N;
    }

    size_type max_size() const
    {
        return N;
    }

    T pop_back()
    {
        assert(!empty());
        auto n = _tail;
        if (tail().link != 0)
        {
            const auto new_tail = tail().link;
            at(new_tail).link ^= _tail;
            _tail = new_tail;
            
        }
        else
        {
            _tail = _head = nil;
        }

        free(n);
        return std::move(at(n).t);
    }

    T pop_front()
    {
        assert(!empty());
        auto n = _head;
        if (head().link != 0)
        {
            const auto new_head = head().link;
            at(new_head).link ^= _head;
            _head = new_head;
        }
        else
        {
            _tail = _head = nil;
        }

        free(n);
        return std::move(at(n).t);
    }


private:
    struct node
    {
        T t;
        offset_type link;
    };

    void init(offset_type n)
    {
        _head = n;
        _tail = n;

        _buf[n].link = 0;
    }
    
    void clear_init()
    {
        for (int i = 0; i < N; ++i)
        {
            _buf[i].link = i + 1;
        }
        _free_list = 0;
        _buf[N - 1].link = nil;
        _head = _tail = nil;
    }

    node& tail()
    {
        return _buf[_tail];
    }

    node& head()
    {
        return _buf[_head];
    }

    node& at(offset_type index)
    {
        return _buf[index];
    }

    const node& tail() const
    {
        return _buf[_tail];
    }

    const node& head() const
    {
        return _buf[_head];
    }

    const node& at(offset_type index) const
    {
        return _buf[index];
    }

    offset_type alloc()
    {
        auto node = _free_list;
        _free_list = _buf[node].link;
        return node;
    }

    void free(offset_type n)
    {
        at(n).link = _free_list;
        _free_list = n;
    }

    offset_type _free_list = 0;
    offset_type _head = nil;
    offset_type _tail = nil; 
    std::array<node, N> _buf;
};

static_assert(sizeof(xorlist<int8_t, 2>) - sizeof(xorlist<int8_t, 1>) == 2, "");
static_assert(sizeof(xorlist<int8_t, 254>) - sizeof(xorlist<int8_t, 253>) == 2, "");
static_assert(sizeof(xorlist<int16_t, 256>) - sizeof(xorlist<int16_t, 255>) == 4, "");
static_assert(sizeof(xorlist<int16_t, 65534>) - sizeof(xorlist<int16_t, 65533>) == 4, "");
static_assert(sizeof(xorlist<int16_t, 65536>) - sizeof(xorlist<int16_t, 65535>) == 8, "");

} // namespace mono
} // namespace haisu
