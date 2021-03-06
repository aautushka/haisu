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
#include <array>
#include "meta.h"

namespace haisu
{
namespace mono
{

template <typename T>
constexpr bool power_of_two(T t)
{
    return t && (!(t & (t - 1)));
}

// a fixed length list, can hold up to N elements
template <typename T, int N>
class queue
{
public:
    using self_type = queue<T, N>;
    using size_type = meta::memory_requirement_t<N>;

    static_assert(power_of_two(N), "suboptimal performance");

    bool empty() const
    {
        return count == 0;
    }
    
    size_type capacity() const
    {
        return N;
    }

    bool full() const
    {
        return size() == capacity();    
    }

    size_type size() const
    {
        return count;
    }

    const T& front() const
    {
        assert(!empty());
        return data[head];
    }

    const T& back() const
    {
        assert(!empty());
        return data[prev(tail)];
    }


    T& front()
    {
        return data[head];
    }

    T& back()
    {
        return data[prev(tail)];
    }

    void clear()
    {
        count = head = tail = 0;
    }

    void push_back(T t)
    {
        assert(!full());

        data[tail] = std::move(t);

        move_tail_forward();
    }

    void push_front(T t)
    {
        assert(!full());

        move_head_backward();

        data[head] = std::move(t);
    }


    T& emplace_back()
    {
        assert(!full());

        T& res = data[tail];
        
        move_tail_forward();
        return res;
    }

    T& emplace_front()
    {
        assert(!full());

        move_head_backward();

        return data[head];
    }

    T pop_back()
    {
        assert(!empty());

        move_tail_backward();

        return std::move(data[tail]);
    }

    T pop_front()
    {
        assert(!empty());

        T t  = std::move(data[head]);
        move_head_forward();

        return std::move(t);
    }

    class iterator
    {
    public:
        using list_t = queue<T, N>;

        iterator(){ }
        iterator(list_t& list) : list(&list), cur(list.head), left(list.size()) { }

        bool operator ==(const iterator& other) const 
        {
            return (list == other.list && cur == other.cur) 
                || (list == nullptr && other.left == 0) 
                || (other.list == nullptr && left == 0); 
        }

        bool operator !=(const iterator& other) const { return !(*this == other); }
        T& operator *() { return list->data[cur]; } 
        const T& operator *() const { return list->data[cur]; }
        iterator& operator ++() { cur = list->next(cur); --left; return *this; }
        iterator operator ++(int) { iterator prev(*this); ++(*this); return prev; }
        T* operator ->() { &**this; }
        const T* operator ->() const { &**this; }

    private:
        list_t* list = nullptr;
        size_type cur = 0;
        size_type left = 0;
    };

    iterator begin() 
    {
        return iterator(*this);
    }

    iterator end()
    {
        return iterator();
    }
private:
    void move_tail_forward()
    {
        ++count;
        move_forward(tail);
    }

    void move_head_backward()
    {
        ++count;
        move_backward(head);
    }

    void move_tail_backward()
    {
        --count;
        move_backward(tail);
    }

    void move_head_forward()
    {
        --count;
        move_forward(head);
    }

    void move_backward(size_type& ptr)
    {
        ptr = ((N - 1) + ptr) % N;
    }

    void move_forward(size_type& ptr)
    {
        ptr = (ptr + 1) % N;
    }

    size_type prev(size_type ptr)
    {
        move_backward(ptr);
        return ptr;
    }

    size_type next(size_type ptr)
    {
        move_forward(ptr);
        return ptr;
    }

    size_type head = 0;
    size_type tail = 0;
    size_type count = 0;
    std::array<T, N> data;
};

} // namespace mono
} // namespace haisu

