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
