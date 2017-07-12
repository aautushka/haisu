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

#include <atomic>
#include <functional>
#include <type_traits>

#include <sys/types.h>

#ifdef __linux__
#ifndef gettid
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif
#elif _WINDOWS
#include "windows.h"
inline uint64_t gettid()
{
    return ::GetCurrentThreadId();
}
#else
inline uint64_t gettid()
{
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return tid;
}
#endif

namespace haisu
{

template <typename T>
struct identity
{
    constexpr T operator ()(T t) const { return t; }
};

template <typename T>
struct amiga_hash
{
    constexpr int32_t operator ()(T t) const
    {
        return t * 0xdeece66d + 0xb;
    }
};

namespace mono
{
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type nil_value()
{
    return 0;
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type nil_value()
{
    return T();
}

// thread-safe fixed-length linear-probing open addressing hash table 
// can't grow, can't rehash
// designed to be as simple as possible and hopefully fast
template <typename Key, typename Value, int N, typename Hash = std::hash<Key>>
class linear_hash
{
public:
    linear_hash()
        : linear_hash(nil_value<Key>())
    {
    }

    explicit linear_hash(const Key& nilval)
        : _nil(nilval)
        , _size(0)
    {
        clear();
    }

    linear_hash(linear_hash&) = delete;
    linear_hash& operator =(linear_hash&) = delete;
    
    ~linear_hash()
    {
    }
    
    Value& operator [](const Key& key)
    {
        return find(key)->value;
    }

    const Value& operator[](const Key& key) const
    {
        return find(key)->value;
    }

    void insert(const Key& key, const Value& value)
    {
        find(key)->value = value;
    }

    void insert(const Key& key, Value&& value)
    {
        find(key)->value = value;
    }

    void erase(const Key& key)
    {
        pair_t* const p = find(key);
        p->value = Value();
        p->key.store(_nil, std::memory_order_relaxed);
        --_size;
    }

    // not thread-safe, better not use it
    bool empty() const
    {
        return 0 == size();
    }

    // not thread-safe, better not use it
    size_t size() const
    {
        return _size;
    }
    
    // not thread-safe
    void clear()
    {
        for (int i = 0; i < N; ++i)
        {
            _data[i].value = Value();
            _data[i].key = _nil;
        }
        _size = 0;
    }

    // not thread-safe
    template <typename Func>
    void foreach(Func func)
    {
        for (int i = 0; i < N; ++i)
        {
            if (_data[i].key != _nil)
            {
                func(_data[i].key.load(), _data[i].value);
            }
        }
    }
        

private:
    struct pair_t
    {
        std::atomic<Key> key;
        Value value;
    };

    pair_t* find(const Key& key)
    {
        int hash = Hash()(key) % N;
        int i = hash;
        do
        {
            pair_t* p = &_data[i];
            i = (i + 1) % N;
            Key prev = p->key.load(std::memory_order_relaxed);
            if (prev != _nil && prev == key)
            {
                return p;
            }
            else if (prev != _nil)
            {
                continue;
            }

            if (p->key.compare_exchange_strong(prev, key, std::memory_order_relaxed))
            {
                ++_size;
                return p;
            }
        }
        while (i != hash);

        // if we get here then the hash is full 
        // you should consider increasing the N parameter or look for a different data structue
        assert(false);
    }

    pair_t _data[N];
    const Key _nil;
    size_t _size;
};
} // namespace mono

// this is a thread local storage implemented on top of a hash container
// the problem with the regular thread_local is its lifespan,
// the thread_local variable lives as long as the thread does,
// if you want to be able to recreate the thread_local object all at once you're in trouble
// the tls class addresses the issue, you may have as many tls objects as you want
// but it comes with a price:it's significantly slower, but sometimes that's okay
// the class is inspired by boost::thread_specific_ptr
// loosely replicates the interface of std::shared_ptr
template <typename T, int N = 131>
class tls
{
public:
    tls()
    {
    }

    explicit tls(T* t)
    {
        reset(t);
    }

    ~tls()
    {
        // TODO
        // do we want to instantly remove all objects in all threads
        // or we allow memory to leak?
        // guess the lifespan of such an object should be greater 
        // than that of any thread
        clear();
    }
    
    tls(const tls&) = delete;
    tls& operator =(const tls&) = delete;

    T& operator *()
    {
        return *get_thread_local();
    }

    const T& operator *() const
    {
        return *get_thread_local();
    }

    T* operator ->()
    {
        return get_thread_local();
    }

    const T* operator ->() const
    {
        return get_thread_local();
    }

    T* release()
    {
        T* out = get_thread_local();
        _hash.erase(get_thread_id());
        return out;
    }

    void reset(T* t = nullptr)
    {
        T*& prev  = get_thread_local();
        delete prev;
        prev = t;
    }

    void clear()
    {
        _hash.foreach([](int32_t, T* t){delete t;});
        _hash.clear();
    }

    T* get()
    {
        return get_thread_local();
    }

    const T* get() const
    {
        return get_thread_local();
    }

private:
    static int32_t get_thread_id()
    {
        return gettid();    
        //return pthread_self();
    }

    T*& get_thread_local()
    {
        return _hash[get_thread_id()];
    }
    
    const T*& get_thread_local() const
    {
        return _hash[get_thread_id()];
    }

    mono::linear_hash<int32_t, T*, N> _hash;  
};
} // namespace haisu 


