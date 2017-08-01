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
#include <cstring>
#include <experimental/string_view>

#include "haisu/mono_stack.h"

// TODO
// null value
// bool value
// integer value
// squote enclosed strings
// escaped quotes inside of a string

namespace haisu
{
namespace json
{

using string_view = std::experimental::string_view;

inline bool is_blank(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

inline bool is_separator(char ch)
{
    return is_blank(ch) || ch == '}' || ch == ']' || ch == ',';
}

inline const char* skip_blanks(const char* str)
{
    while (is_blank(*str)) ++str;
    return str;
}

inline const char* skip_to(char ch, const char* str)
{
    while (*str && *str != ch) ++str;
    return str;
}

inline bool preceded_by_even_number_of_backslashes(const char* str)
{
    int ret = 1;
    while (*str-- == '\\')
    {
        ret ^= 1;
    }
    
    return ret;
}

inline const char* skip_to_end_of_string(char quote, const char* str)
{
    while (true)
    {
        str = skip_to(quote, str);
        if (*str && str[-1] == '\\' && preceded_by_even_number_of_backslashes(str - 2))
        {
            ++str;    
            continue;
        }
        break;
    }
    return str;
}

template <char ch>
const char* skip_to(const char* str)
{
    return skip_to(ch, str);
}

template <char ch>
const char* skip_past(const char* str)
{
    auto ret = skip_to<ch>(str);
    return ret + (*ret ? 1 : 0);
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

template <bool B, typename T>
auto call_bool_value(T& t, int) -> decltype(t.on_value(B), void())
{
    t.on_value(B);
}

template <bool B, typename T> void call_bool_value(T&t, long) {}

template <bool B, typename T>
auto call_bool_array(T& t, int) -> decltype(t.on_array(B), void())
{
    t.on_array(B);
}

template <bool B, typename T> void call_bool_array(T& t, long) {}

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

template <typename T, int N>
class static_stack
{
public:
    using size_type = meta::memory_requirement_t<N>;
    T top() const noexcept
    {
        assert(cur_ < N);
        return stack_[cur_];
    }

    template <T val>
    void push() noexcept 
    {
        assert(cur_ > 0);
        stack_[--cur_] = val;
    }

    void push(T t) noexcept
    {
        assert(cur_ > 0);
        stack_[--cur_] = t;
    }

    void pop() noexcept
    {
        assert(cur_ < N);
        ++cur_;
    }

    T empty() const noexcept
    {
        return cur_ == N;
    }

private:
    T stack_[N];
    size_type cur_ = N;
};

template <int N>
using boolstack = mono::stack<bool, N>;

template <int N>
class bitstack
{
    enum { storage_size = N % 64 ? N / 64 + 1 : N / 64 }; 
public:
    bool empty() const
    {
        return pos_ < 0;
    }

    template <bool Flag>
    void push()
    {
        mask_ <<= 1;
        if (!mask_)
        {
            mask_ = 1;
            ++pos_;
            bits_[pos_] = 0;
        }

        using tag = std::conditional_t<Flag, std::true_type, std::false_type>;
        push_flag(tag());
    }

    void push(bool flag)
    {
        if (flag)
        {
            push<true>();
        }
        else
        {
            push<false>();
        }
    }

    void pop()
    {
        assert(!empty());
        mask_ >>= 1;
        if (!mask_)
        {
            mask_ = 0x8000000000000000;
            --pos_;
        }
    }

    bool top() const
    {
        assert(!empty());
        return bits_[pos_] & mask_;
    }

    constexpr int capacity() const
    {
        return N;
    }

private:
    void push_flag(std::true_type)
    {
        bits_[pos_] |= mask_; 
    }

    void push_flag(std::false_type)
    {
    }

    uint64_t bits_[storage_size];
    uint64_t mask_ = 0;
    int pos_ = -1;
};

template <>
class bitstack<63>
{
public:
    constexpr bool top() const
    {
        return bits_ & mask_;
    }

    void pop()
    {
        mask_ >>= 1;
    }

    template <bool Flag>
    void push()
    {
        mask_ <<= 1;
        using tag = std::conditional_t<Flag, std::true_type, std::false_type>;
        push_flag(tag());
    }

    constexpr int capacity() const
    {
        return 63;
    }

private:
    void push_flag(std::true_type)
    {
        bits_ |= mask_;
    }

    void push_flag(std::false_type)
    {
        bits_ &= ~mask_;
    }

    uint64_t bits_ = 0;
    uint64_t mask_ = 1;
};

template <int N>
class nibblestack
{
public:
    using size_type = meta::memory_requirement_t<N / 2>;

    bool empty() const
    {
        return size_ == 0;
    }

    bool top() const
    {
        assert(size_ > 0);
        return bits_[size_ - 1] & mask_;
    }

    template <bool Flag>
    void push()
    {
        size_ += ((mask_ ^ 0xf0) + 1);
        mask_ ^= 0xff;
        
        using tag = std::conditional_t<Flag, std::true_type, std::false_type>;
        push_flag(tag());
    }

    void pop()
    {
        size_ -= ((mask_ ^ 0x0f) + 1);
        mask_ ^= 0xff;
    }

private:
    static_assert(0 == N % 2, "");

    void push_flag(std::true_type)
    {
        bits_[size_ - 1] |= (mask_ & 0xff);
    }

    void push_flag(std::false_type)
    {
        bits_[size_ - 1] &= (mask_ ^ 0xff);
    }

    size_type size_ = 0;
    uint8_t mask_ = 0xf0;
    uint8_t bits_[N / 2];
};

template <int N>
class objstack
{
public:
    void push_array() noexcept
    {
        stack_.template push<true>();
    }

    void push_object() noexcept
    {
        stack_.template push<false>();
    }

    bool is_object_on_top() const noexcept
    {
        return !stack_.top();    
    }

    bool is_array_on_top() const noexcept
    {
        return stack_.top();
    }

    void pop() noexcept
    {
        stack_.pop();
    }

    bool empty() const noexcept
    {
        return stack_.empty();
    }

private:
    boolstack<N> stack_;
};

template <int N>
class compressed_objstack
{
public:
    void push_array()
    {
        if (is_array_on_top())
        {
            assert(stack_[size_] != 0x7f);
            ++stack_[size_];
        }
        else
        {
            assert(size_ < N);
            ++size_;
            stack_[size_] = 0x01;
        }
    }

    void push_object()
    {
        if (is_object_on_top())
        {
            assert(stack_[size_] != 0xff);
            ++stack_[size_];
        }
        else
        {
            assert(size_ < N);
            ++size_;
            stack_[size_] = 0x81;
        }
    }

    void pop()
    {
        assert(size_ >= 0);    
        stack_[size_] -= 1;
        if (!(stack_[size_] & 0x7f))
        {
            --size_;
        }
    }

    bool is_object_on_top() const
    {
        return stack_[size_] & 0x80;
    }

    bool is_array_on_top() const
    {
        return !is_object_on_top();
    }
    

private:
    int size_ = -1;
    uint8_t stack_[N];    
};

template <typename T>
class parser
{
    enum parser_state
    {
        state_object_key,
        state_object_value,
        state_array_item,
        state_bad
    };

public:
    void start_object();
    void end_object();
    
    void start_array();
    void end_array();
    
    void parse(const char* s)
    {
        static_stack<int8_t, 64> stack;
        parser_state state = state_bad;
        stack.push(state_bad);

        do
        {
            s = skip_blanks(s);
            
            switch (*s)
            {
                case '{': // new object
                    stack.push<state_object_key>();
                    state = state_object_key;
                    call_on_new_object();
                    break;
                case '[': // new array
                    stack.push<state_array_item>();
                    state = state_array_item;
                    call_on_new_array();
                    break;
                case '}': // object end
                    call_on_object_end();
                    stack.pop();
                    state = static_cast<parser_state>(stack.top());
                    break;
                case ']': // array end
                    call_on_array_end();
                    state = static_cast<parser_state>(stack.top());
                    stack.pop();
                    break;
                case ',':
                    state = static_cast<parser_state>(stack.top());
                    break;
                case ':':
                    state = state_object_value;
                    break;
                case '\'': // object key, or array item
                case '"': // object key, or array item
                    {
                        const auto quote = *s;
                        const auto k = ++s;
                        s = skip_to_end_of_string(quote, s);
                        switch (state)
                        {
                            case state_object_key:
                                call_on_key(k, s);
                                break;
                            case state_object_value:
                                call_on_value(k, s);
                                break;
                            case state_array_item:
                                call_on_array(k, s);
                                break;
                        }
                    }
                    break;
                case 'n': // null
                    if (s[1] == 'u' && s[2] == 'l' && s[3] == 'l' && is_separator(s[4]))
                    {
                        switch (state)
                        {
                            case state_object_value: call_on_null_value(); break;
                            case state_array_item: call_on_null_array(); break;
                        }
                        s += 4;
                    }
                    break;
                case 't': // true
                    if (s[1] == 'r' && s[2] == 'u' && s[3] == 'e' && is_separator(s[4]))
                    {
                        switch (state)
                        {
                            case state_object_value: call_on_bool_value<true>(); break;
                            case state_array_item: call_on_bool_array<true>(); break;
                        }
                        s += 3;
                    }
                    break;
                case 'f': // false
                    if (s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e' && is_separator(s[5]))
                    {
                        switch (state)
                        {
                            case state_object_value: call_on_bool_value<false>(); break;
                            case state_array_item: call_on_bool_array<false>(); break;
                        }
                        s += 5;
                    }
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '-':
                    ++s;
                    while (*s >= '0' && *s <= '9') ++s;
                    break;
            }
            ++s;
        }
        while (*s);
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

    template <bool B>
    void call_on_bool_value()
    {
        call_bool_value<B>(*static_cast<T*>(this), 0);
    }

    template <bool B>
    void call_on_bool_array()
    {
        call_bool_array<B>(*static_cast<T*>(this), 0);
    }

    void call_on_int_value()
    {
    }

    void call_on_int_array()
    {
    }
    
    void call_on_null_value()
    {
        //call_null_value(*static_cast<T*>(this));
    }

    void call_on_null_array()
    {
        //call_null_array(*static_cast<T*>(this));
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

    void on_value(bool)
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

