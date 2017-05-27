#pragma once
#include <cstring>

#include "haisu/mono_stack.h"

namespace haisu
{
namespace json
{

const char* skip_blanks(const char* str)
{
	while (*str == ' ') ++str;
	return str;
}

template <char ch>
const char* skip_to(const char* str)
{
	return strchr(str, ch);
}

template <char ch>
const char* skip_past(const char* str)
{
	return 1 + strchr(str, ch);
}

template <typename T>
class dquote
{
public:
	enum {trigger = '"'};

	void parse(const char*& str)
	{
	}

private:
};

template <typename T>
class quote
{
public:
	enum {trigger = '\''};

	void parse(const char*& str)
	{
	}

private:
};

template <typename T>
auto call_key(T& t, const char* first, const char* last, int) -> decltype(t.on_key(first, last), void())
{
	t.on_key(first, last);
}

template <typename T>
void call_key(T&t, const char* first, const char* last, long)
{
}

template <typename T>
auto call_value(T& t, const char* first, const char* last, int) -> decltype(t.on_value(first, last), void())
{
	t.on_value(first, last);
}

template <typename T>
void call_value(T&t, const char* first, const char* last, long)
{
}

template <typename T>
auto call_array(T& t, const char* first, const char* last, int) -> decltype(t.on_array(first, last), void())
{
	t.on_array(first, last);
}

template <typename T> void call_array(T&, const char*, const char*, long) { }

template <typename T>
auto call_new_object(T& t, int) -> decltype(t.on_new_object(), void())
{
	t.on_new_object();
}

template <typename T> void call_new_object(T&, long) {}

template <typename T>
auto call_new_array(T& t, int) -> decltype(t.on_new_array(), void())
{
	t.on_new_array();
}

template <typename T> void call_new_array(T& t, long) {}

template <typename T>
auto call_object_end(T& t, int) -> decltype(t.on_object_end(), void())
{
	t.on_object_end();
}

template <typename T> void call_object_end(T&, long) {}

template <typename T>
auto call_array_end(T& t, int) -> decltype(t.on_array_end(), void())
{
	t.on_array_end();
}

template <typename T> void call_array_end(T&, long) {}

template <int N>
class bitstack
{
public:
	using size_type = meta::memory_requirement_t<N>;
	bool top() const
	{
		assert(size_ > 0);
		return bits_[size_ - 1];
	}

	void push(bool b)
	{
		assert(size_ < N);
		bits_[size_++] = b;
	}

	void pop()
	{
		assert(size_ > 0);
		--size_;
	}

	bool empty() const
	{
		return !size_;
	}

private:
	size_type size_ = 0;
	bool bits_[N];
};

template <typename T>
class parser
{
	enum state { ARR, OBJ };
	enum key_val { KEY, VAL };

public:
	void start_object();
	void end_object();
	
	void start_array();
	void end_array();
	
	void parse(const char* str)
	{
		bitstack<10> depth;
		const char* cur = str;
		key_val kv = KEY;
loop:
		do
		{
			cur = skip_blanks(cur);

			switch (*cur)
			{
				case '{': // new object
					kv = KEY;
					call_on_new_object();
					depth.push(OBJ);
					break;
				case '[': // new array
					kv = KEY;
					call_on_new_array();
					depth.push(ARR);
					break;
				case '"': // object key, or array item
					{
					auto k = ++cur;
					cur = skip_to<'"'>(cur);
					if (depth.top() == OBJ)
					{
						if (kv == KEY)
						{
							call_on_key(k, cur);
						}
						else 
						{
							call_on_value(k, cur);
							kv = KEY;
						}
					}
					else
					{
						call_on_array(k, cur);
					}
					}
					break;
				case '}': // object end
					call_on_object_end();
					depth.pop();
					break;
				case ']': // array end
					call_on_array_end();
					depth.pop();
					break;
				case ',':
					break;
				case ':':
					kv = VAL;
					break;
			}
			++cur;
		}
		while (!depth.empty());
	}

private:
	void call_on_key(const char* str, const char* end)
	{
		call_key(*static_cast<T*>(this), str, end, 0);
	}

	void call_on_value(const char* str, const char* end)
	{
		call_value(*static_cast<T*>(this), str, end, 0);
	}

	void call_on_array(const char* str, const char* end)
	{
		call_array(*static_cast<T*>(this), str, end, 0);
	}

	void call_on_new_object()
	{
		call_new_object(*static_cast<T*>(this), 0);
	}

	void call_on_new_array()
	{
		call_new_array(*static_cast<T*>(this), 0);
	}

	void call_on_object_end()
	{
		call_object_end(*static_cast<T*>(this), 0);
	}

	void call_on_array_end()
	{
		call_array_end(*static_cast<T*>(this), 0);
	}

	void parse_array(const char*& cur)
	{
		auto prev = ++cur;
		cur = skip_to<'"'>(cur);
		call_on_array(prev, cur++);
	}
};

class model : public parser<model>
{
public:
	void on_key(const char* str, const char* end)
	{
	}

	void on_value(const char* str, const char* end)
	{
	}

	void on_array(const char* str, const char* end)
	{
	}

	void on_new_object()
	{
	}

	void on_new_array()
	{
	}

	void on_object_end()
	{
	}

	void on_array_end()
	{
	}
private:
};

} // namespace json
} // namespace haisu

