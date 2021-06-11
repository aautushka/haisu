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
        static_assert(meta::one_of<U, Ts...>{}, "");

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
        static_assert(meta::one_of<U, Ts...>{}, "");

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
        static_assert(meta::one_of<U, Ts...>{}, "");
        dealloc_no_type_check(u);
    }

    template <typename U>
    void destroy(U* u)
    {
        static_assert(meta::one_of<U, Ts...>{}, "");
        assert(belongs(u));

        assert(belongs(u));
        u->~U();
        dealloc(u);
    }

    template <typename U>
    bool belongs(const U* u) const
    {
        static_assert(meta::one_of<U, Ts...>{}, "");

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


