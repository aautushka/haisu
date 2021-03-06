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

#include <typeinfo>

namespace haisu
{
class bad_any_cast : public std::bad_cast
{
};

template <int N>
class stack_any
{
public:
    stack_any()
    {
        set_empty_vtable();
    }

    stack_any(const stack_any& rhs)
        : vtable_(rhs.vtable_)
    {
        vtable_.copy(rhs.storage_, storage_);
    }

    stack_any& operator =(const stack_any& rhs)
    {
        vtable_ = rhs.vtable_;
        vtable_.destroy(storage_);
        vtable_.copy(rhs.storage_, storage_);
        return *this;
    }

    stack_any(stack_any&& rhs)
        : vtable_(rhs.vtable_)
    {
        vtable_.move(rhs.storage_, storage_);
    }

    stack_any& operator =(stack_any&& rhs)
    {
        vtable_ = rhs.vtable_;
        vtable_.move(rhs.storage_, storage_);
        return *this;
    }

    ~stack_any()
    {
        clear();
    }

    void clear()
    {
        vtable_.destroy(storage_);
        set_empty_vtable();
    }

    template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, stack_any>{}>>
    stack_any(T&& t)
    {
        construct(std::forward<T>(t));
    }

    template <typename T>
    T& unsafe_cast()
    {
        assert(vtable_.type() == typeid(T));
        return *reinterpret_cast<T*>(&storage_);
    }

    template <typename T>
    T& cast()
    {
        if (vtable_.type() == typeid(T))
        {
            return unsafe_cast<T>();
        }
        

        throw bad_any_cast();
    }

    template <typename T>
    void reset(T&& t)
    {
        vtable_.destroy(storage_);
        construct(std::forward<T>(t));
    }

    const std::type_info& type() const noexcept
    {
        return vtable_.type();
    }

    bool empty() const noexcept
    {
        return vtable_.destroy == empty_dtor;
    }

    void swap(stack_any& rhs)
    {
        stack_any tmp(std::move(rhs));
        rhs = std::move(*this);
        *this = std::move(tmp);
    }

private:
    using storage_type = std::aligned_storage_t<N, std::alignment_of<void*>::value>;

    struct vtable
    {
        void (*destroy)(storage_type& storage);
        const std::type_info& (*type)();
        void (*copy)(const storage_type& from, storage_type& to);
        void (*move)(storage_type& from, storage_type& to);
    };


    template <typename T>
    static const std::type_info& get_type_id()
    {
        return typeid(T);
    }

    template <typename T>
    static void destroy(storage_type& storage)
    {
        auto p = reinterpret_cast<T*>(&storage);
        p->~T();
    }
    
    static void empty_dtor(storage_type&)
    {
    }

    template <typename T>
    static void copy(const storage_type& from, storage_type& to)
    {
        auto pfrom = reinterpret_cast<const T*>(&from);
        auto pto = reinterpret_cast<T*>(&to);

        new (pto) T(*pfrom);
    }

    template <typename T>
    static void move(storage_type& from, storage_type& to)
    {
        auto pfrom = reinterpret_cast<T*>(&from);
        auto pto = reinterpret_cast<T*>(&to);

        new (pto) T(std::move(*pfrom));
    }

    void set_empty_vtable()
    {
        vtable_.destroy = empty_dtor;
        vtable_.type = get_type_id<void>;
        vtable_.copy = [](const auto&, auto&){};
        vtable_.move = [](auto&, auto&){};
    }

    template <typename T>
    void construct(T&& t)
    {
        using U = std::decay_t<T>;
        static_assert(sizeof(U) <= sizeof(storage_type), "");

        U* p = reinterpret_cast<U*>(&storage_);
        new (p) U(std::forward<T>(t));

        create_vtable<U>();
    }

    template <typename T>
    void create_vtable()
    {
        vtable_.destroy = destroy<T>;
        vtable_.type = get_type_id<T>;
        vtable_.copy = copy<T>;
        vtable_.move = move<T>;
    }

    storage_type storage_;
    vtable vtable_;
};

using small_any = stack_any<2 * sizeof(void*)>;

} // namespace haisu


