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

namespace haisu 
{
namespace meta
{

//
//memory_requirement_bytes
//
template <int N> struct memory_requirement_bytes;
template <> struct memory_requirement_bytes<1> { using type = uint8_t; };
template <> struct memory_requirement_bytes<2> { using type = uint16_t; };
template <> struct memory_requirement_bytes<4> { using type = uint32_t; };

template <int N> struct calc_memory
{
    enum { result = (N < 255 ? 1 : (N < 65535 ? 2 : 4)) };
};

template <int N>
struct memory_requirement
{
    using type = typename memory_requirement_bytes<calc_memory<N>::result>::type;
};

template <int N> using memory_requirement_t = typename memory_requirement<N>::type;

//
// one_of
//
template <typename ... Ts> struct one_of;
template <typename T> struct one_of<T> : std::integral_constant<bool, false> {};
template <typename T, typename ... Ts> struct one_of<T, T, Ts...> : std::integral_constant<bool, true> {};
template <typename T, typename U, typename ... Ts> struct one_of<T, U, Ts...> : std::integral_constant<bool, one_of<T, Ts...>::value>{};

static_assert(one_of<int, int>::value, "");
static_assert(one_of<int, char, short, int>::value, "");
static_assert(!one_of<int, char>::value, "");

//
// max_mem
//
template <int N, class ... Ts> struct max_mem : std::integral_constant<int, N> {};

template <int N, class T, class ... Ts>
struct max_mem<N, T, Ts...> : std::integral_constant<int, max_mem<(N > sizeof(T) ? N : sizeof(T)), Ts...>::value> {};

static_assert(4 == max_mem<0, int32_t>::value, "");
static_assert(4 == max_mem<0, int16_t, int32_t>::value, "");
static_assert(4 == max_mem<0, int32_t, int16_t>::value, "");
static_assert(8 == max_mem<0, int8_t, int16_t, int32_t, int64_t>::value, "");

//
// all_trivial_types
// 
template <typename ... Ts>
struct all_trivial_types;

template <>
struct all_trivial_types<> : std::integral_constant<bool, true> {};

template <typename T, typename ...Ts>
struct all_trivial_types<T, Ts...> : std::integral_constant<bool, std::is_trivially_copyable<T>{} && all_trivial_types<Ts...>{}> {};

static_assert(all_trivial_types<int, bool, double>{});
static_assert(!all_trivial_types<int, std::string>{});

//
// index_of
// 
template <typename ... Ts>
struct index_of;

template <> struct index_of<> : std::integral_constant<int, -1>{};

template <typename T, typename ... Ts>
struct index_of<T, T, Ts...> : std::integral_constant<int, 0> {};

template <typename T, typename U, typename ... Ts>
struct index_of<T, U, Ts...> : std::integral_constant<int, 1 + index_of<T, Ts...>{}> {};

static_assert(0 == index_of<int, int>{});
static_assert(1 == index_of<int, char, int>{});
static_assert(2 == index_of<int, char, long, int>{});

} // namespace meta
} // namespace haisu

