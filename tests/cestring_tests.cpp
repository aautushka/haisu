#include <gtest/gtest.h>

#include <algorithm>

class cestring
{
public:
	constexpr cestring() : _ptr(""), _size(0) {}

	template <size_t Len>
	constexpr cestring(const char(&ptr)[Len]) : _ptr(ptr), _size(Len - 1) {}

	constexpr cestring( const char* ptr) : _ptr(ptr), _size(strlen(ptr)) { }

	constexpr const char* c_str() const
	{
		return _ptr;
	}

	constexpr size_t size() const
	{
		return _size;
	}

	constexpr bool operator <(const cestring& other) const
	{
		return strcmp(_ptr, other._ptr) < 0;
	}

	constexpr bool operator ==(const cestring& other) const
	{
		return 0 == strcmp(_ptr, other._ptr);
	}

	constexpr bool operator !=(const cestring& other) const
	{
		return !(*this == other);
	}

private:
	const char* _ptr;
	const size_t _size;
};


template <class T>
constexpr T cemin(T t)
{
	return t;
}

template <class T, class... Ts>
constexpr T cemin(T lhs, T rhs, Ts... ts)
{
	return lhs < rhs ? cemin(lhs, ts...) : cemin(rhs, ts...);
}


template <class... Ts>
struct celist
{
};

template <class T, class... Ts>
struct celist<T, Ts...> : public celist<Ts...>
{
	celist(T t, Ts... ts)
		: celist<Ts...>(ts...)
	{
	}

	T t;
};

template <class... Ts>
celist<Ts...> generate(Ts... ts)
{
	return celist<Ts...>(ts...);
}

template <class... Ts>
struct strlist
{
};

template <class T, class... Ts>
struct strlist<T, Ts...> : public strlist<Ts...>
{
	using base_type = strlist<Ts...>;

	constexpr strlist(T t, Ts... ts)
		: strlist<Ts...>(ts...)
		, t(t)
	{
	}

	template <int N> constexpr cestring get() const
	{
		return N == 0 ? t : this->base_type::get<0>();
	}

	cestring t;
};

template <class... Ts>
constexpr strlist<Ts...> generate_strlist(Ts... ts)
{
	return strlist<Ts...>(ts...);
}

TEST(cestring_test, compares_strings_at_compile_time)
{
	static_assert(cestring("hello") < cestring("world"), "");
	static_assert(cestring("hello") == cestring("hello"), "");
	static_assert(cestring("hello") != cestring("world"), "");
}

TEST(cestring_test, finds_string_len_at_compile_time)
{
	static_assert(cestring("hello").size() == 5, "");
}

TEST(cestring_test, finds_min_value_at_compile_time)
{
	static_assert(1 == cemin(5, 4, 3, 2, 1), "");
}

TEST(cestring_test, sorts_strlist_at_compile_time)
{
	generate_strlist("world", "hello").get<0>();

	//static_assert(generate_strlist("world", "hello").get<0>() == cestring("hello"), "");
}

