#pragma once

#include "haisu/meta.h"

namespace haisu
{

template <class ... Ts>
struct memory_holder
{
    uint8_t memory[meta::max_mem<0, Ts...>::value];

    template <typename U>
    U* cast_memory()
    {
        static_assert(meta::one_of<U, Ts...>{});
        return reinterpret_cast<U*>(memory);
    }

    template <typename U>
    const U* cast_memory() const
    {
        static_assert(meta::one_of<U, Ts...>{});
        return reinterpret_cast<const U*>(memory);
    }

    template <typename U, typename ... Args>
    U* construct(Args&&... args)
    {
        static_assert(meta::one_of<U, Args...>{});
        new (memory) U(std::forward(args)...);
    }

};

} // namespace haisu 


