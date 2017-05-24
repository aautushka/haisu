#pragma once

namespace haisu 
{
namespace meta
{

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

} // namespace meta
} // namespace haisu

