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

