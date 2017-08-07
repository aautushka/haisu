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
#include "haisu/memory_holder.h"
#include "haisu/meta.h"

namespace haisu
{

template <int N, class ... Ts>
class heterogeneous_pool
{
public:
    static_assert(N > 0, "empty pool is not allowed");

    heterogeneous_pool()
    {
        create_free_list();
    }

    ~heterogeneous_pool()
    {
    }

    void dealloc_all()
    {
        uint8_t free_objects[capacity()] = {0};
        auto o = free_;

        while (o)
        {
            auto index = o - pool_;
            free_objects[index] = 1;
            o = o->next;
        }

        for (int i = 0; i < capacity(); ++i)
        {
            if (!free_objects[i])
            {
                dealloc_no_type_check(&pool_[i].obj);
            }
        }
    }

    template <typename U>
    U* alloc()
    {
        static_assert(meta::one_of<U, Ts...>::value, "");

        if (free_)
        {
            auto ret = free_->obj.template cast_memory<U>();

            free_ = free_->next;
            return ret;
        }

        return nullptr;
    }

    template <typename U, typename ... Args>
    U* construct(Args&& ... args)
    {
        static_assert(meta::one_of<U, Ts...>::value, "");

        if (free_)
        {
            auto ret = free_->obj.template cast_memory<U>();
            free_ = free_->next;
            new (ret) U(std::forward<Args>(args)...); 
            return ret;
        }

        return nullptr;
    }

    template <typename U>
    void dealloc(U* u)
    {
        static_assert(meta::one_of<U, Ts...>::value, "");
        dealloc_no_type_check(u);
    }

    template <typename U>
    void destroy(U* u)
    {
        static_assert(meta::one_of<U, Ts...>::value, "");
        assert(belongs(u));

        assert(belongs(u));
        u->~U();
        dealloc(u);
    }

    template <typename U>
    bool belongs(const U* u) const
    {
        static_assert(meta::one_of<U, Ts...>::value, "");

        auto o = reinterpret_cast<const object*>(u);
        return o >= pool_ && o <= &pool_[N - 1];
    }

    static constexpr size_t capacity()
    {
        return N;
    }

    size_t size() const
    {
        auto o = free_;
        auto free_size = 0;
        while (o)
        {
            ++free_size;
            o = o->next;
        }
        return capacity() - free_size;
    }

private:
    template <typename U>
    void dealloc_no_type_check(U* u)
    {
        auto o = reinterpret_cast<object*>(u);
        o->next = free_;
        free_ = o;    
    }

    void create_free_list()
    {
        for (int i = 0; i < N - 1; ++i)
        {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[N - 1].next = nullptr;
        free_ = &pool_[0];
    }

    union object
    {
        memory_holder<Ts...> obj;    
        object* next;

        object() {}
        ~object() {}
    };

    object* free_ = nullptr;
    object pool_[N];
};
    
} // namespace haisu


