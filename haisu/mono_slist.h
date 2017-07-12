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
#include "meta.h"

namespace haisu
{
namespace mono 
{

template <typename T, int N, typename Offset = meta::memory_requirement_t<N>>
class slist
{
    using offset_type = Offset; 
    enum {nil = std::numeric_limits<offset_type>::max()};
    static_assert(std::numeric_limits<offset_type>::max() >= N, "");
public:
    using size_type = offset_type;
    
    slist()
    {
        clear_init();
    }

    slist(const slist&) = delete;
    slist& operator =(const slist&) = delete;
    slist(const slist&&) = delete;
    slist& operator =(const slist&&) = delete;

    slist(std::initializer_list<T> ll)
    {
        *this = std::move(ll);
    }

    slist& operator =(std::initializer_list<T> ll)
    {

        for (auto l: ll)
        {
            push_back(std::move(l));
        }

        return *this;
    }

    void clear()
    {
        // TODO: this does not call destructors
        if (_head != nil)
        {
            tail().next = _free_list;
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
            _buf[node].next = nil;
            
            tail().next = node;
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
            _buf[node].next = _head;
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
        auto cur = _head;
        while (cur != _tail)
        {
            res += 1;
            cur = at(cur).next;    
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

        auto prev = find_prev(_tail);
        auto old_tail = _tail;
        if (prev != nil)
        {
            at(prev).next = nil;
            _tail = prev;
        }
        else
        {
            _tail = _head = nil;
        }

        free(old_tail);
        return std::move(at(old_tail).t);
    }

    T pop_front()
    {
        assert(!empty());
        auto n = _head;
        if (head().next != nil)
        {
            _head = head().next;
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
        offset_type next;
    };

    offset_type find_prev(offset_type n)
    {
        auto prev = _head;

        while (prev != nil && at(prev).next != _tail)
        {
            prev = at(prev).next;
        }

        return prev;
    }

    void init(offset_type n)
    {
        _head = n;
        _tail = n;

        _buf[n].next = nil;
    }
    
    void clear_init()
    {
        for (int i = 0; i < N; ++i)
        {
            _buf[i].next = i + 1;
        }
        _free_list = 0;
        _buf[N - 1].next = nil;
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
        _free_list = _buf[node].next;
        return node;
    }

    void free(offset_type n)
    {
        at(n).next = _free_list;
        _free_list = n;
    }

    offset_type _free_list = 0;
    offset_type _head = nil;
    offset_type _tail = nil; 
    node _buf[N];
};

static_assert(sizeof(slist<int8_t, 2>) - sizeof(slist<int8_t, 1>) == 2, "");
static_assert(sizeof(slist<int8_t, 254>) - sizeof(slist<int8_t, 253>) == 2, "");
static_assert(sizeof(slist<int16_t, 256>) - sizeof(slist<int16_t, 255>) == 4, "");
static_assert(sizeof(slist<int16_t, 65534>) - sizeof(slist<int16_t, 65533>) == 4, "");
static_assert(sizeof(slist<int16_t, 65536>) - sizeof(slist<int16_t, 65535>) == 8, "");

} // namespace mono
} // namespace haisu

