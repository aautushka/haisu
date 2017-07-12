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

// https://gist.github.com/dabrahams/1457531
// https://github.com/eliben/code-for-blog/blob/master/2014/variadic-tuple.cpp
// http://eli.thegreenplace.net/2014/variadic-templates-in-c/
// https://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/
// http://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c

#include <gtest/gtest.h>

#include <algorithm>

#define static_assert(Expr) static_assert(Expr, "")

//namespace compile
//{

// 
// compile-type c-string wrapper
// 
class string
{
public:
	constexpr string() : _ptr(""), _size(0) {}

	template <size_t Len>
	constexpr string(const char(&ptr)[Len]) : _ptr(ptr), _size(Len - 1) {}

	constexpr string( const char* ptr) : _ptr(ptr), _size(strlen(ptr)) { }

	constexpr const char* c_str() const
	{
		return _ptr;
	}

	constexpr size_t size() const
	{
		return _size;
	}

	constexpr bool operator <(const string& other) const
	{
		return strcmp(_ptr, other._ptr) < 0;
	}

	constexpr bool operator ==(const string& other) const
	{
		return 0 == strcmp(_ptr, other._ptr);
	}

	constexpr bool operator !=(const string& other) const
	{
		return !(*this == other);
	}

private:
	const char* _ptr;
	const size_t _size;
};

static_assert(string("hello") < string("world"));
static_assert(string("hello") == string("hello"));
static_assert(string("hello") != string("world"));
static_assert(string("hello").size() == 5);


//
// finds min value
//
template <class T>
constexpr T min(T t)
{
	return t;
}

template <class T, class... Ts>
constexpr T min(T lhs, T rhs, Ts... ts)
{
	return lhs < rhs ? min(lhs, ts...) : min(rhs, ts...);
}

static_assert(1 == min(5, 4, 3, 2, 1));


//
// tuple containing only strings
//
template <class... Ts>
struct struple { };

template <>
struct struple<>
{
	template <int N> constexpr string get() const
	{
		return string();
	}
};

template <class T, class... Ts>
struct struple<T, Ts...> : public struple<Ts...>
{
	using base_type = struple<Ts...>;

	constexpr struple(T t, Ts... ts)
		: struple<Ts...>(ts...)
		, t(t)
	{
	}

	template <int N> constexpr string get() const
	{
		return N == 0 ? t : base_type::template get<N-1>();
	}

	string t;
};

template <class... Ts>
constexpr struple<Ts...> make_struple(Ts... ts)
{
	return struple<Ts...>(ts...);
}

static_assert(make_struple("world", "hello").get<0>() == string("world"));
static_assert(make_struple("world", "hello").get<1>() == string("hello"));
static_assert(make_struple("world", "hello").get<3>() == string());


//
// compile-type list
//
struct nil 
{
	constexpr size_t size() const { return 0; }

	template <typename T>
	constexpr bool operator ==(const T& other) const 
	{
		return false;
	}

	template <typename T>
	constexpr bool operator !=(const T& other) const
	{
		return !*(this == other);
	}
};

template <> inline constexpr bool nil::operator ==(const nil& other) const { return true;}

template <typename H, typename T = nil>
struct cons_type
{
	using head_type = H;
	using tail_type = T;

	H head;
	T tail;

	constexpr cons_type()
	{
	}

	constexpr cons_type(H h, T t)
		: head(h), tail(t)
	{
	}

	constexpr size_t size() const
	{
		return 1 + tail.size();
	}

	template <class U, class V>
	constexpr bool operator ==(const cons_type<U, V> &other)
	{
		return head == other.head && tail == other.tail;
	}

	template <class U, class V>
	constexpr bool operator !=(const cons_type<U, V>& other)
	{
		return !(*this == other);
	}
};

template <typename H, typename T>
constexpr cons_type<H, T> make_cons(H h, T t)
{
	return cons_type<H, T>(h, t);
}

template <class... Ts> struct type {};

template <class H, class... Ts>
struct type<H, Ts...>
{
	using head = H;
	using tail = typename type<Ts...>::get;
	using get = cons_type<head, tail>;
};

template <class H>
struct type<H>
{
	using head = H;
	using tail = nil;
	using get = cons_type<head, tail>;
};

template <class T, class... Ts>
using cons_t = typename type<T, Ts...>::get;

constexpr nil cons()
{
	return nil();
}

template <class T, class... Ts>
constexpr cons_t<T, Ts...> cons(T t, Ts... ts)
{
	return cons_t<T, Ts...>(t, cons(ts...));
}


constexpr auto c1 = cons(1);
constexpr auto c2 = cons(1, 2);
constexpr auto c3 = cons(1, 2, 3);

static_assert(cons(1).size() == 1);
static_assert(cons(1, 2).size() == 2);
static_assert(cons(1, 2, 3).size() == 3);

static_assert(cons(1) == cons(1));
static_assert(cons(1, 2, 3) == cons(1, 2, 3));

static_assert(cons(1, 2, 3) != cons(1, 2, 5));
static_assert(cons(1, 2, 3) != cons(1, 2, 3, 4));


template <class T, class V>
constexpr bool same(T t, V v)
{
	return false;
}

template <class T>
constexpr bool same(T lhs, T rhs)
{
	return lhs == rhs;
}

static_assert(same(1, 1) == true);
static_assert(same(1, 2) == false);
static_assert(same(1, 1.0) == false);

template <typename ConsT, bool = true>
struct remove_head 
{
	using get = typename ConsT::tail_type;
};

template <typename ConsT>
struct remove_head<ConsT, false>
{
	using get = ConsT;
};

template <typename Cons, typename Func>
constexpr Cons transform(Cons c, Func f)
{
	return Cons(f(c.head), transform(c.tail, f));
}

template <typename Func>
constexpr nil transform(nil, Func)
{
	return nil();
}

constexpr int increment(int t) { return t + 1; }
static_assert(transform(cons(1), increment) == cons(2));

template <typename Cons>
Cons bubble_sort(Cons c)
{
	return c;
}

auto a = bubble_sort(cons(1, 2, 3));

