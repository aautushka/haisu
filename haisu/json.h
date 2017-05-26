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
class parser
{
	enum state { ARR, OBJ };

public:
	void start_object();
	void end_object();
	
	void start_array();
	void end_array();
	
	void key(const char* str, const char* end)
	{
		static_cast<T*>(this)->key(str, end);
	}

	void value(const char* str, const char* end)
	{
		static_cast<T*>(this)->value(str, end);
	}

	void array(const char* str, const char* end)
	{
		static_cast<T*>(this)->array(str, end);
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
						key(k, cur++);
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
						value(v, cur++);
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
		++cur;
		auto prev = ++cur;
		skip_to<'"'>(cur);
		array(prev, cur++);
	}
};

class model : public parser<model>
{
public:
	void key(const char* str, const char* end)
	{
		key_.assign(str, end);
	}

	void value(const char* str, const char* end)
	{
		pairs_[key_] = std::string(str, end);
		key_.clear();
	}

	std::string operator [](const std::string& key) const
	{
		auto i = pairs_.find(key);
		if (i != pairs_.end())
		{
			return i->second;
		}

		return "";
	}

private:
	std::map<std::string, std::string> pairs_;
	std::string key_;
};

} // namespace json
} // namespace haisu

