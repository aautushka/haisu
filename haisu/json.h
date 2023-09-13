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

// clang-format off
#pragma once
#include <cstring>
#include <string_view>
#include <array>

#include "haisu/meta.h"
#include "haisu/mono_stack.h"

// TODO:
// full validation on demand

namespace haisu
{
namespace json
{

template <typename Expr, typename Var>
constexpr auto is_valid_expression_impl(int) -> decltype(std::declval<Expr>()(std::declval<Var>()), bool())
{
    return true;
}

template <typename Expr, typename Var>
constexpr bool is_valid_expression_impl(long)
{
    return false;
}

template <typename Var, typename Expr>
constexpr bool is_valid_expression(Expr&&, Var&&)
{
    return is_valid_expression_impl<Expr, Var>(0);
}

template <typename Var, typename Expr>
constexpr bool is_valid_expression(Expr&&)
{
    return is_valid_expression_impl<Expr, Var>(0);
}

static_assert(is_valid_expression([](auto&& o) -> decltype(o.size()) {}, std::array<int, 10>()), "");
static_assert(!is_valid_expression([](auto&& o) -> decltype(o.clear()) {}, std::array<int, 10>()), "");

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


template <typename T, int N>
class static_stack
{
public:
    using size_type = meta::memory_requirement_t<N>;
    const T& top() const noexcept
    {
        assert(cur_ < N);
        return stack_[cur_];
    }

    T& top() noexcept
    {
        assert(cur_ < N);
        return stack_[cur_];
    }

    template <T val>
    void push() noexcept 
    {
        assert(!full());

        stack_[--cur_] = val;
    }

    void push(T t) noexcept
    {
        assert(!full());

        stack_[--cur_] = t;
    }

    void pop() noexcept
    {
        assert(cur_ < N);
        ++cur_;
    }

    bool empty() const noexcept
    {
        return cur_ == N;
    }

    bool full() const noexcept
    {
        return cur_ == 0;
    }

    size_type size() const noexcept
    {
        return N - cur_;
    }

    void clear() noexcept
    {
        cur_ = N;
    }

    static size_type capacity() noexcept
    {
        return N;
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

enum class error_code
{
    json_too_deep_to_parse, // if you get this error, consider increasing the MaxDepth parameter in the parser
    malformed_json, 
    unexpected_character,
    unspecified_error
};

using string_view = std::string_view;

struct string_literal
{
    string_view view;
};

struct null_literal 
{
};

struct bool_literal 
{
    bool value;
};

struct numeric_literal 
{ 
    string_view view; 
};

struct error 
{ 
    const char* position; 
    error_code err; 
};

// A minimalistic JSON parser with following characteristics
//     1) makes no memory allocations (but it uses stack memory alright)
//     2) builds no DOM
//     3) the parser is quite close to SAX philosophy, it provides a stream of events instead of DOM
//     3) provides very limited validation
//     4) relies on a CRTP derivee to sort out how it wants to handle the json
//     5) a derivee may terminate parser at any moment by calling terminate()
//     6) the maximum depth of json being parsed is limited
//     7) does no string unescaping, relies on the derivee
//     8) does not parse numbers, numbers are presented as strings, passes that to the derivee
//     9) same for unicode, let the derivee handle this
//     10) the main focus of this parser is performance, it should be easily customizable when performance is at stake
//        and some features may be left out (if I don't want doubles, why bother parsing them anyway?)
template <typename T, int MaxDepth = 64>
class parser
{
    static_assert(MaxDepth > 0, "MaxDepth is not allowed to be zero");
    enum parser_state : int8_t
    {
        state_object_key,
        state_object_value,
        state_array_item,
        state_bad
    };

public:
    // the input string must be null-terminated
    void parse(const char* json_string)
    {
        parser_state state = state_bad;

        const auto transit = [&](auto new_state) {
            state = new_state;
            stack_.push(static_cast<int8_t>(new_state));
        };

        const auto transform = [&](auto new_state) {
            state = new_state;
            stack_.top() = static_cast<int8_t>(new_state);
        };

        const auto restore_previous_state = [&] {
            stack_.pop();
            state = static_cast<parser_state>(stack_.top());
        };

        stack_.clear();
        feed_ = json_string;
        transit(state_bad);

        auto& s = feed_;

#ifdef DEBUG_JSON_PARSER
        const auto state_str = [](auto state) {
            switch (state) {
                case state_bad:
                    return "bad";
                case state_object_key:
                    return "key";
                case state_array_item:
                    return "arr";
                case state_object_value:
                    return "val";
            }
        };
#endif

        do
        {
            s = skip_blanks(s);

#ifdef DEBUG_JSON_PARSER
            auto prev_state = state;
            auto ch = s[0];
#endif
            
            switch (*s)
            {
                case '{': // new object
                    switch (state) {
                        case state_object_value:
                            transform(state_object_key);
                            break;
                        case state_bad:
                        case state_array_item:

                            if constexpr (has_error_handler())
                            {
                                if (stack_.full())
                                {
                                    return call_on_error({s, error_code::json_too_deep_to_parse});
                                }
                            }
                            transit(state_object_key);
                            break;
                        default:
                            break;
                    }
                    call_on_new_object();
                    break;
                case '[': // new array
                    switch (state) {
                        case state_object_value:
                            transform(state_array_item);
                            break;
                        case state_array_item:
                        case state_bad:
                            if constexpr (has_error_handler())
                            {
                                if (stack_.full())
                                {
                                    return call_on_error({s, error_code::json_too_deep_to_parse});
                                }
                            }
                            transit(state_array_item);
                            break;
                        default:
                            break;
                    }
                    call_on_new_array();
                    break;
                case '}': // object end
                    if constexpr (!has_error_handler()) // assume no errors possible
                    {
                        call_on_object_end();
                        restore_previous_state();
                        break;
                    }
                    else
                    {
                        if (stack_.size() >= 2)
                        {
                            call_on_object_end();
                            restore_previous_state();
                            break;
                        }
                        else
                        {
                            return call_on_error({s, error_code::malformed_json});
                        }
                    }
                case ']': // array end
                    if constexpr (!has_error_handler()) // assume no errors possible
                    {
                        call_on_array_end();
                        restore_previous_state();
                        break;
                    }
                    else
                    {
                        if (stack_.size() >= 2)
                        {
                            call_on_array_end();
                            restore_previous_state();
                            break;
                        }
                        else
                        {
                            return call_on_error({s, error_code::malformed_json});
                        }
                    }
                case ',':
                    if constexpr (has_error_handler())
                    {
                        if (stack_.size() <= 1)
                        {
                            return call_on_error({s, error_code::malformed_json});
                        }
                    }

                    break;
                case ':':
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

                                if constexpr (has_error_handler())
                                {
                                    if (stack_.full())
                                    {
                                        return call_on_error({s, error_code::json_too_deep_to_parse});
                                    }
                                }

                                transit(state_object_value);
                                break;
                            case state_object_value:
                                call_on_value(k, s);
                                restore_previous_state();
                                break;
                            case state_array_item:
                                call_on_array(k, s);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case 'n': // null
                    if (s[1] == 'u' && s[2] == 'l' && s[3] == 'l' && is_separator(s[4]))
                    {
                        switch (state)
                        {
                            case state_object_value: 
                                call_on_null_value(); 
                                restore_previous_state();
                                break;
                            case state_array_item: 
                                call_on_null_array(); 
                                break;
                            default:
                                break;
                                                
                        }
                        s += 3;
                        break;
                    }
                    else if constexpr (has_error_handler())// malformed literal
                    {
                        return call_on_error({s, error_code::unexpected_character});
                    }
                case 't': // true
                    if (s[1] == 'r' && s[2] == 'u' && s[3] == 'e' && is_separator(s[4]))
                    {
                        switch (state)
                        {
                            case state_object_value: 
                                call_on_bool_value<true>(); 
                                restore_previous_state();
                                break;
                            case state_array_item: 
                                call_on_bool_array<true>(); 
                                break;
                            default:
                                break;
                        }
                        s += 3;
                        break;
                    }
                    else if constexpr (has_error_handler()) // malformed literal
                    {
                        return call_on_error({s, error_code::unexpected_character});
                    }
                case 'f': // false
                    if (s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e' && is_separator(s[5]))
                    {
                        switch (state)
                        {
                            case state_object_value: 
                                call_on_bool_value<false>(); 
                                restore_previous_state();
                                break;
                            case state_array_item: 
                                call_on_bool_array<false>(); break;
                            default:
                                break;
                        }
                        s += 4;
                        break;
                    }
                    else if constexpr (has_error_handler()) // malformed literal
                    {
                        return call_on_error({s, error_code::unexpected_character});
                    }
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
                    {
                        const auto k = s;
                        do
                        {
                            ++s;
                        }
                        while ((*s >= '0' && *s <= '9') || *s == '.' || *s =='e');

                        if constexpr (has_error_handler())
                        {
                            if (!is_separator(*s))
                            {
                                return call_on_error({s, error_code::unexpected_character});
                            }
                        }

                        // TODO: extract function
                        const auto literal_ptr = k;
                        const auto literal_size = size_t(s - k);
                        auto literal = numeric_literal{string_view{literal_ptr, literal_size}};
                        switch (state)
                        {
//                        TODO: do we allow numeric keys?
//                        case state_object_key:
//                            call_on_key(k, s);
//                            break;
//
                            case state_object_value:
                                call_on_value(literal);
                                restore_previous_state();
                                break;
                            case state_array_item:
                                call_on_array(literal);
                                break;
                            default:
                                break;
                        }
                        s -= 1;
                    }

                    break;
                default:
                    if constexpr (has_error_handler())
                    {
                        return call_on_error(s);
                    }
            }

#ifdef DEBUG_JSON_PARSER
            std::cout << ch << " : " << state_str(prev_state) << " --> " << state_str(state) << " : " << (int)stack_.size()  << std::endl;
#endif

            ++s;
        }
        while (*s);

        if constexpr (has_error_handler())
        {
            if (stack_.size() != 1 || stack_.top() != state_bad)
            {
                call_on_error(s);
            }
        }
    }

protected:
    void terminate()
    {
        feed_ = "";
    }

private:
    void call_on_key(const char* str, const char* end)
    {
        const auto view = string_view(str, end - str);
        call_key(*static_cast<T*>(this), string_literal{view}, 0);
    }

    void call_on_value(const char* str, const char* end)
    {
        const auto view = string_view(str, end - str);
        call_value(*static_cast<T*>(this), string_literal{view}, 0);
    }

    void call_on_array(const char* str, const char* end)
    {
        const auto view = string_view(str, end - str);
        call_array(*static_cast<T*>(this), string_literal{view}, 0);
    }

    template <bool B>
    void call_on_bool_value()
    {
        call_value(*static_cast<T*>(this), bool_literal{B}, 0);
    }

    template <bool B>
    void call_on_bool_array()
    {
        call_array(*static_cast<T*>(this), bool_literal{B}, 0);
    }

    void call_on_value(numeric_literal lit)
    {
        call_value(*static_cast<T*>(this), lit, 0);
    }

    void call_on_array(numeric_literal lit)
    {
        call_array(*static_cast<T*>(this), lit, 0);
    }
    
    void call_on_null_value()
    {
        call_value(*static_cast<T*>(this), null_literal{}, 0);
    }

    void call_on_null_array()
    {
        call_array(*static_cast<T*>(this), null_literal{}, 0);
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

    void call_on_error(const char* pos)
    {
        call_on_error(error{pos, error_code::unspecified_error});
    }

    void call_on_error(error err)
    {
        call_error(*static_cast<T*>(this), err, 0);
    }

    static constexpr bool has_error_handler() noexcept
    {
        return is_valid_expression<T>([](auto&& o) -> decltype(o.on_error(error{})) {});
    }

    template <typename U, typename Literal>
    static auto call_key(U& t, Literal&& lit, int) -> decltype(t.on_key(std::forward<Literal>(lit)), void())
    {
        t.on_key(std::forward<Literal>(lit));
    }

    template <typename U, typename Literal>
    static void call_key(U&t, Literal&&, long)
    {
    }

    template <typename U, typename Literal>
    static auto call_value(U& t, Literal&& lit, int) -> decltype(t.on_value(std::forward<Literal>(lit)), void())
    {
        t.on_value(std::forward<Literal>(lit));
    }

    template <typename U, typename Literal>
    static void call_value(U&t, Literal&&, long)
    {
    }

    template <typename U, typename Literal>
    static auto call_array(U& t, Literal&& lit, int) -> decltype(t.on_array(std::forward<Literal>(lit)), void())
    {
        t.on_array(std::forward<Literal>(lit));
    }

    template <typename U, typename Literal> 
    static void call_array(U&, Literal&&, long) { }

    template <typename U>
    static auto call_new_object(U& t, int) -> decltype(t.on_new_object(), void())
    {
        t.on_new_object();
    }

    template <typename U> 
    static void call_new_object(U&, long) {}

    template <typename U>
    static auto call_new_array(U& t, int) -> decltype(t.on_new_array(), void())
    {
        t.on_new_array();
    }

    template <typename U> static void call_new_array(U& t, long) {}

    template <typename U>
    static auto call_object_end(U& t, int) -> decltype(t.on_object_end(), void())
    {
        t.on_object_end();
    }

    template <typename U> static void call_object_end(U&, long) {}

    template <typename U>
    static auto call_array_end(U& t, int) -> decltype(t.on_array_end(), void())
    {
        t.on_array_end();
    }

    template <typename U> static void call_array_end(U&, long) {}

    template <typename U, typename Error>
    static auto call_error(U& t, Error&& err, int) -> decltype(t.on_error(std::forward<Error>(err)), void())
    {
        t.on_error(std::forward<Error>(err));
    }

    template <typename U, typename Error> static void call_error(U&, Error&&, long) {}


    static_stack<int8_t, MaxDepth + 1> stack_; // one element on the stack is reserved
    const char* feed_;
};

} // namespace json
} // namespace haisu
