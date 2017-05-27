#pragma once

#include "haisu/mono_stack.h"

namespace haisu
{
namespace json
{

void skip_blanks(const char*& str)
{
	while (*str == ' ') ++str;
}

template <char ch>
void skip_to(const char*& str)
{
	str = strchr(str, ch);
}

template <char ch>
void skip_past(const char*& str)
{
	str = 1 + strchr(str, ch);
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

template <typename T>
void call_array(T&t, const char* first, const char* last, long)
{
}

template <typename T>
class parser
{
	enum state { ARR, OBJ };

public:
	void start_object();
	void end_object();
	
	void start_array();
	void end_array();
	
	void on_key(const char* str, const char* end)
	{
		call_key(*static_cast<T*>(this), str, end, 0);
	}

	void on_value(const char* str, const char* end)
	{
		call_value(*static_cast<T*>(this), str, end, 0);
	}

	void on_array(const char* str, const char* end)
	{
		call_array(*static_cast<T*>(this), str, end, 0);
	}

	void parse(const char* str)
	{
		mono::stack<int8_t, 10> depth;
		const char* cur = str;

		skip_blanks(cur);
		if (*cur == '{')
		{
			depth.push(OBJ);
		}
		else if (*cur == '[')
		{
			depth.push(ARR);
		}
		++cur;

loop:
		while (!depth.empty())
		{
			skip_blanks(cur);
			if (depth.top() == OBJ)
			{
				switch (*cur)
				{
					case '"': // key
					{
						const char* const k = ++cur;
						skip_to<'"'>(cur);
						on_key(k, cur++);
						break;
					}
					case '}': // end of object
					{
						++cur;
						depth.pop();
						goto loop;
					}
					case ',': // next key-value pair
					{
						++cur;
						goto loop;
					}
					case '[': // new array
					{
						++cur;
						depth.push(ARR);
						continue;
					}
				}

				skip_past<':'>(cur);
				skip_blanks(cur);

				switch (*cur)
				{
					case '{': // new object
					{
						skip_blanks(cur);
						++cur;
						depth.push(OBJ);
						goto loop;
					}
					case '[': // new array
					{
						skip_blanks(cur);
						++cur;
						depth.push(ARR);
						goto loop;
					}
					case '"': // new value
					{
						const char* const v = ++cur;
						skip_to<'"'>(cur);
						on_value(v, cur++);
						break;
					}
				}
			}
			else if (depth.top() == ARR)
			{
				switch (*cur)
				{
					case '"':
					{
						parse_array(cur);
						break;
					}
					case ']':
					{
						++cur;
						depth.pop();
						goto loop;
					}
					case ',':
					{
						++cur;
						goto loop;
					}
					case '[':
					{
						++cur;
						depth.push(ARR);
						goto loop;
					}
					case '{':
					{
						++cur;
						depth.push(OBJ);
						goto loop;
					}
				}
			}
		}
	}

private:
	void parse_array(const char*& cur)
	{
		auto prev = ++cur;
		skip_to<'"'>(cur);
		on_array(prev, cur++);
	}
};

} // namespace json
} // namespace haisu

