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

