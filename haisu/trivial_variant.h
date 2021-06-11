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
#include <type_traits>

#include "haisu/memory_holder.h"

namespace haisu
{

class bad_variant_access : std::exception {};

// a trivial conterpart of std::variant, allowed to hold trivial types only
// can be serialized with memcpy and constructed just by castring a pointer
template <typename ... Ts>
class trivial_variant 
{
public:
    using self_type = trivial_variant<Ts...>;

    static_assert(meta::all_trivial_types<Ts...>{});

    constexpr std::size_t index() const noexcept
    {
        return index_;
    }

    template <typename T>
    T& get() 
    {
        static_assert(meta::one_of<T, Ts...>{});
        if (index_ == meta::index_of<T, Ts...>::value)
        {
            return *memory_.template cast_memory<T>();
        }
        throw bad_variant_access{};
    }

    template <typename T>
    const T& get() const
    {
        static_assert(meta::one_of<T, Ts...>{});
        if (index_ == meta::index_of<T, Ts...>::value)
        {
            return *memory_.template cast_memory<T>();
        }
        throw bad_variant_access{};
    }

    template <typename T>
    T* get_if() noexcept
    {
        static_assert(meta::one_of<T, Ts...>{});
        if (index_ == meta::index_of<T, Ts...>::value)
        {
            return memory_.template cast_memory<T>();
        }
        return nullptr;
    }

    template <typename T>
    const T* get_if() const noexcept
    {
        static_assert(meta::one_of<T, Ts...>{});
        if (index_ == meta::index_of<T, Ts...>::value)
        {
            return memory_.template cast_memory<T>();
        }
        return nullptr;
    }

    template <typename T>
    T& select() noexcept
    {
        static_assert(meta::one_of<T, Ts...>{});
        index_ = meta::index_of<T, Ts...>{};
        return *memory_.template cast_memory<T>();
    }

    template <typename T>
    void assign(T&& t) noexcept
    {
        select<std::decay_t<T>>() = t;
    }

    template <typename T, typename = std::enable_if_t<!std::is_same<T, self_type>{}>>
    void operator =(T&& t) noexcept
    {
        assign(t);
    }

private:
    std::size_t index_{};
    memory_holder<Ts...> memory_;
};

static_assert(std::is_trivially_copyable<trivial_variant<int, double>>{});

} // namespace haisu
