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
#include <functional>

namespace haisu
{

namespace detail
{

template <typename T>
union pooled_object
{
    T obj;
    pooled_object* next;

    pooled_object() {}
    ~pooled_object() {}
};

} // namespace detail

template <typename T, int N>
class object_pool final
{
    using object = detail::pooled_object<T>;

public:
    static_assert(N > 0, "empty pool is not allowed");
    object_pool()
    {
        create_free_list();
    }

    ~object_pool()
    {
        // it's the user responsibility to destroy the allocated object
        // when we go out of scope and the object is not destroyed, the pointer becomes invalidated
        // however since the desctructor is not called, resource leaks are possible
    }

    void destroy_all()
    {
        foreach_object([this](auto o){destroy(o);});
    }

    void dealloc_all()
    {
        foreach_object([this](auto o){dealloc(o);});
    }

    T* alloc()
    {
        if (free_)
        {
            auto ret = &free_->obj;
            free_ = free_->next;
            return ret;
        }

        return nullptr;
    }

    template <typename ... Args>
    T* construct(Args&& ... args)
    {
        if (free_)
        {
            auto ret = &free_->obj;
            free_ = free_->next;
            new (ret) T(std::forward<Args>(args)...); 
            return ret;
        }

        return nullptr;
    }

    void dealloc(T* t)
    {
        assert(belongs(t));
        auto o = reinterpret_cast<object*>(t);
        o->next = free_;
        free_ = o;    
    }

    void destroy(T* t)
    {
        t->~T();
        dealloc(t);
    }

    bool belongs(const T* t) const
    {
        return t >= &pool_[0].obj && t <= &pool_[N - 1].obj;
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
    void create_free_list()
    {
        for (int i = 0; i < N - 1; ++i)
        {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[N - 1].next = nullptr;
        free_ = &pool_[0];
    }

    template <typename Visitor>
    void foreach_object(Visitor&& f)
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
                f(&pool_[i].obj);
            }
        }
    }

    object* free_ = nullptr;
    object pool_[N];
};

// pool always grows and never shrinks
// allocates memory on heap
template <typename T>
class heap_pool final
{
public:
    using size_type = std::size_t;

    heap_pool() = default; 
    heap_pool(heap_pool&&) = default;
    heap_pool& operator =(heap_pool&&) = default;

    ~heap_pool() noexcept
    {
        free_memory();
    }

    T* alloc() noexcept
    {
        if (flist_ == nullptr)
        {
            create_new_slab();

            if (!flist_)
            {
                return nullptr;
            }
        }

        auto ret = remove_free_list_head();
        ++size_;
        return ret;
    }

    void dealloc(T* t) noexcept
    {
        auto o = reinterpret_cast<object*>(t);
        o->next = flist_;
        flist_ = o;
        --size_;
    }

    template <typename ... Args>
    T* construct(Args&& ... args) 
    {
        T* o = alloc();
        if (o)
        {
            new (o) T(std::forward<Args>(args)...);
        }
        return o;
    }


    void destroy(T* t)
    {
        t->~T();
        dealloc(t);
    }

    heap_pool(const heap_pool&) = delete;
    heap_pool& operator =(const heap_pool&) = delete;

    size_type capacity() const noexcept
    {
        return capacity_;
    }

    size_type size() const noexcept
    {
        return size_;
    }

    void dealloc_all() noexcept
    {
        auto s = head_;
        object* flist = nullptr;
        while (s)
        {
            auto f = create_free_list(s);
            f.second->next = flist;
            flist = f.first;
            s = s->next;
        }
        flist_ = flist;
        size_ = 0;
    }

private:
    using object = detail::pooled_object<T>;
    
    struct slab
    {
        slab* next;
        int32_t count;
        object data[1];
    };


    T* remove_free_list_head() noexcept
    {
        assert(flist_ != nullptr);
        auto ret = flist_;
        flist_ = flist_->next;
        return &ret->obj;
    }

    void free_memory() noexcept
    {
        auto cur = head_;
        while (cur)
        {
            const auto next = cur->next;
            free(cur);
            cur = next;
        }
    }

    void create_new_slab() noexcept
    {
        assert(slab_size_ >= 0);

        auto size = (slab_size_ - 1) * sizeof(object) + sizeof(slab);
        auto p = static_cast<slab*>(malloc(size)); // TODO: alignment
        if (p)
        {
            p->count = slab_size_;
            p->next = nullptr;

            enlist_new_slab(p);
            flist_ = create_free_list(p).first;

            capacity_ += slab_size_;
            increase_next_slab_size(size);
        }
    }

    void enlist_new_slab(slab* s) noexcept
    {
        if (tail_)
        {
            tail_->next  = s;
        }
        else
        {
            head_ = tail_ = s;
        }
    }

    void increase_next_slab_size(size_t current_size) noexcept
    {
        slab_size_ = current_size > 4 * 1024 ? slab_size_ : slab_size_ * 2;
    }

    static std::pair<object*, object*> create_free_list(slab* s)
    {
        auto flist = s->data;
        auto prev = flist;

        for (auto i = 1; i < s->count; ++i)
        {
            prev->next = &s->data[i];
            prev = prev->next;
        }

        prev->next = nullptr;
        return {flist, prev};
    }

    slab* head_ = nullptr;
    slab* tail_ = nullptr;
    object* flist_ = nullptr;
    uint16_t slab_size_ = 1;
    size_type capacity_{};
    size_type size_{};
};
    
} // namespace haisu


