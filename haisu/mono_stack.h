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
#include <cstring>
#include "meta.h"

namespace haisu
{
namespace mono
{

template <typename T, int N = 256>
class stack final
{
public:
    using size_type = meta::memory_requirement_t<N>;

    static_assert(std::is_integral<T>::value, "integral type required");
    static_assert(N <= std::numeric_limits<T>::max(), "");

    class iterator
    {
    public:
        iterator() {} 
        explicit iterator(stack& parent) : _parent(&parent) { }
        T& operator *() {return _parent->_stack[_cursor]; }
        const T& operator *() const {return _parent->_stack[_cursor];}
        iterator& operator ++() { ++_cursor; return *this; }
        iterator operator ++(int) { iterator out(*this); ++(*this); return out;}
        T* operator ->() { return &**this; }
        const T* operator ->() const {return &**this;}

        bool operator !=(const iterator& other) const { return !(*this == other); }
        bool operator ==(const iterator& other) const 
        {
            if (_parent && other._parent)
            {
                assert(_parent == other._parent);
                return _cursor == other._cursor;
            }

            return end() && other.end();
        }

    private:
        bool end() const
        {
            return !_parent || _cursor < 0 || _cursor >= _parent->size();
        }

        stack* _parent = nullptr;
        int _cursor = 0;
    };

    class const_iterator
    {
    public:
        using self = const_iterator;

        const_iterator() {} 
        explicit const_iterator(const stack& parent) : _parent(&parent) { }
        const T& operator *() {return _parent->_stack[_cursor]; }
        const T& operator *() const {return _parent->_stack[_cursor];}
        self& operator ++() { ++_cursor; return *this; }
        self operator ++(int) { self out(*this); ++(*this); return out;}
        const T* operator ->() { return &**this; }
        const T* operator ->() const {return &**this;}

        bool operator !=(const self& other) const { return !(*this == other); }
        bool operator ==(const self& other) const 
        {
            if (_parent && other._parent)
            {
                assert(_parent == other._parent);
                return _cursor == other._cursor;
            }

            return end() && other.end();
        }

    private:
        bool end() const
        {
            return !_parent || _cursor < 0 || _cursor >= _parent->size();
        }

        const stack* _parent = nullptr;
        int _cursor = 0;
    };

    stack()
    {
        memset(this, 0, sizeof(*this));
    }

    stack(const stack& other)
    {
        memcpy(this, &other, sizeof(other));
    }

    stack& operator =(const stack& other)
    {
        memcpy(this, &other, sizeof(other));
        return *this;
    }

    stack(std::initializer_list<T> ll)
        : stack()
    {
        for (auto l : ll) push(l);
    }

    template <int U>
    bool operator ==(const stack<T, U>& other) const
    {
        if (_size == other._size)
        {
            return 0 == memcmp(_stack, other._stack, byte_capacity());
        }
        return false;
    }

    bool operator ==(const stack& other) const
    {
        return 0 == memcmp(this, &other, sizeof(*this));
    }

    template <int U>
    bool operator !=(const stack<T, U>& other) const
    {
        return !(*this == other);
    }

    void operator +=(T item)
    {
        push(item);
    }
    
    template <int U>
    void operator +=(const stack<T, U>& other)
    {
        assert(left() >= other.size());

        memcpy(_stack + _size, other._stack, sizeof(T) * other._size);
        _size += other._size;
    }

    bool operator <(const stack& other) const
    {
        return 0 > memcmp(_stack, other._stack, byte_capacity());
    }

    T top() const
    {
        assert(_size > 0 && _size <= N);

        return _stack[_size - 1];
    }

    void pop()
    {
        assert(_size > 0);

        _stack[--_size] = 0;
    }

    void push(T t)
    {
        assert(_size < N);
        _stack[_size++] = t;
    }

    template <T t>
    void push()
    {
        assert(_size < N);
        _stack[_size++] = t;
    }

    iterator begin() 
    {
        return iterator(*this); 
    }

    iterator end() 
    {
        return iterator();
    }

    const_iterator cbegin() const
    {
        return const_iterator(*this);
    }

    const_iterator cend() const
    {
        return const_iterator();
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

    size_type size() const
    {
        return _size;
    }

    bool empty() const
    {
        return 0 == size();
    }

    static constexpr size_type capacity()
    {
        return N;
    }
    
    const T& front() const
    {
        assert(_size > 0);
        return _stack[0];
    }

    const T& back() const
    {
        assert(_size > 0);
        return _stack[_size - 1];
    }
    
    T& front()
    {
        assert(_size > 0);
        return _stack[0];
    }

    T& back()
    {
        assert(_size > 0);
        return _stack[_size - 1];
    }

    bool full() const
    {
        return _size == capacity();
    }

private:
    size_type left() const
    {
        return capacity() - _size;
    }

    static constexpr size_type byte_capacity()
    {
        return capacity() * sizeof(T);
    }

    size_type _size = 0;
    T _stack[N];

    template <typename U, int V> friend class stack;
};

static_assert(sizeof(stack<char, 15>) == 16, "the structure must be tightly packed"); 

// overflow-tolerant stack, ignores everything what goes beyond the boundary
template <typename T, int N = 256>
class overflow_stack final
{
public:
    using self_type = overflow_stack;
    using base_type = stack<T, N>;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using size_type = typename base_type::size_type; 

    overflow_stack()
    {
    }

    overflow_stack(std::initializer_list<T> ll)
    {
        for (auto l : ll) push(l);
    }

    void push(T t)
    {
        if (_stack.size() < _stack.capacity())
        {
            _stack.push(t);
        }
        else
        {
            ++_overflow;
        }
    }

    void pop()
    {
        if (overflow())
        {
            --_overflow;
        }
        else
        {
            _stack.pop();
        }
    }

    T top()
    {
        assert(_stack.size() > 0 && !overflow());
        return _stack.top();
    }
    
    size_type size() const
    {
        return _stack.size() + _overflow;
    }

    bool empty()
    {
        return 0 == size();
    }

    static constexpr size_type capacity()
    {
        return N;
    }

    T& front()
    {
        return _stack.front();
    }

    const T& front() const
    {
        return _stack.front();
    }

    T& back()
    {
        assert(!overflow());
        return _stack.back();
    }

    const T& back() const
    {
        assert(!overflow());
        return _stack.back();
    }

    bool overflow() const
    {
        return _overflow > 0;
    }

    bool full() const
    {
        return size() >= capacity();
    }
    
    template <int M>
    bool operator ==(const overflow_stack<T, M>& other) const
    {
        // completely ignore the overflow part here
        return _stack == other._stack;
    }

    template <int M>
    bool operator !=(const overflow_stack<T, M>& other) const
    {
        // completely ignore the overflow part here
        return _stack != other._stack;
    }

    template <int M>
    bool operator <(const overflow_stack<T, M>& other) const
    {
        // completely ignore the overflow part here
        return _stack < other._stack;
    }

    void operator +=(T t)
    {
        push(t);
    }

    template <int M>
    void operator +=(const overflow_stack<T, M>& other)
    {
        // TODO: not efficient
        for (auto i = other._stack.begin(); i != other._stack.end(); ++i)
        {
            push(*i);
        }

        _overflow += other._overflow;
    }

    iterator begin()
    {
        assert(!overflow());
        return _stack.begin();
    }

    iterator end()
    {
        return _stack.end();
    }

    const_iterator begin() const
    {
        assert(!overflow());
        return _stack.begin();
    }

    const_iterator end() const
    {
        return _stack.end();
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    const_iterator cend() const
    {
        return end();
    }

private:
    base_type _stack;
    size_type _overflow = 0;

    template <typename U, int M> friend class overflow_stack;
};

} // namespace mono
} // namespace haisu

