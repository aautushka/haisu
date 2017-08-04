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
#include "haisu/json.h"
#include "haisu/zbuf.h"

#include <variant>

namespace haisu
{
namespace json
{

namespace detail
{

struct list_node
{
    size_t next;
};

struct object : list_node
{
    string_literal key;
};

struct array : list_node
{
    size_t next;
};

// cant' use std::variant because it is not trivially copyable
struct jsonval
{
    enum value_type
    {
        type_object,
        type_array,
        type_string,
        type_null,
        type_number,
        type_unknown
    };

    union
    {
        object obj;
        array arr;
        string_literal str;
        null_literal null;
        numeric_literal num;
    } val;

    value_type type;

    void assign(object o) noexcept
    {
        val.obj = o;
        type = type_object;
    }

    void assign(string_literal s) noexcept
    {
        val.str = s;
        type = type_string;
    }

    void assign(array a) noexcept
    {
        val.arr = a;
        type = type_array;
    }

    template <typename T> 
    const T* get_if() const noexcept;

    template <typename T>
    const T& get() const noexcept;

    template <typename F>
    void visit(F&& f) const
    {
        switch (type)
        {
            case type_object:
                f(val.obj);
                break;
            case type_string:
                f(val.str);
                break;
            case type_array:
                f(val.arr);
                break;
            case type_null:
                f(val.null);
                break;
            case type_number:
                f(val.num);
                break;
        }
    }
};

template<>
const object& jsonval::get<object>() const noexcept
{
    assert(type == type_object);
    return val.obj;
}

template <>
const string_literal& jsonval::get<string_literal>() const noexcept
{
    assert(type == type_string);
    return val.str;
}

template <>
const array& jsonval::get<array>() const noexcept
{
    assert(type == type_array);
    return val.arr;
}

template <>
const object* jsonval::get_if<object>() const noexcept
{
    return type == type_object ? &get<object>() : nullptr;
}

template <>
const string_literal* jsonval::get_if<string_literal>() const noexcept
{
    return type == type_string ? &get<string_literal>() : nullptr;
}

template <>
const array* jsonval::get_if<array>() const noexcept
{
    return type == type_array ? &get<array>() : nullptr;
}

class model_view
{
public:
    model_view(const zbuf& buffer, size_t offset)
        : buffer_(buffer)
        , offset_(offset)
    {
    }

    explicit model_view(const zbuf& buffer)
        : buffer_(buffer)
        , offset_(buffer.size())
    {
    }

    bool has_key(const char* key) const noexcept
    {
        if (is_object())
        {
            auto offset = offset_;
            do
            {
                const auto o = get_object(offset);
                if (!o->key.view.compare(key))
                {
                    return true;
                }
                offset = o->next;
            }
            while (offset);
        }
        return false;
    }

    model_view child(const char* key) const noexcept
    {
        if (is_object())
        {
            auto offset = offset_;
            do
            {
                const auto o = get_object(offset);
                if (!o->key.view.compare(key))
                {
                    auto child_offset = offset + sizeof(jsonval);
                    if (get_object(child_offset))
                    {
                        return model_view{buffer_, offset + sizeof(jsonval)};
                    }
                }
                offset = o->next;
            }
            while (offset);
        }
        return model_view{buffer_};
    }

    auto count() const noexcept
    {
        auto ret = size_t{};
        foreach_child([&ret](auto&&){ ++ret; });
        return ret;
    }

    auto count_array() const noexcept
    {
        auto ret = size_t{};
        if (is_array())
        {
            auto off = offset_;
            auto a = get<array>(off);
            while (a->next)
            {
                ++ret;
                off = a->next;
                a = get<array>(off);
            }
        }

        return ret;
    }

    template <typename F>
    void foreach_child(F&& f) const noexcept
    {
        if (is_object())
        {
            auto offset = offset_;
            do
            {
                f(model_view{buffer_, offset});
                const auto o = get_object(offset);
                offset = o->next;
            }
            while (offset);
        }
    }

    bool empty() const noexcept
    {
        return buffer_.size() <= offset_;
    }

    bool is_object() const noexcept
    {
        return !empty() && get_object(offset_);
    }

    bool is_literal() const noexcept
    {
        return !empty() && !get_object(offset_);
    }

    bool is_string() const noexcept
    {
        return !empty() && get_if<string_literal>(offset_);
    }

    bool is_array() const noexcept
    {
        return !empty() && get_if<array>(offset_);
    }

private:
    const object* get_object(size_t offset) const noexcept
    {
        return get_if<object>(offset);
    }

    template <typename T>
    const T* get_if(size_t offset) const noexcept
    {
        auto& v = buffer_.at_offset<jsonval>(offset);
        return v.get_if<T>();
    }

    template <typename T>
    const T* get(size_t offset) const noexcept
    {
        auto& v = buffer_.at_offset<jsonval>(offset);
        return &v.get<T>();
    }
    
    const object* get_root() const noexcept
    {
        return get_object(offset_);
    }

    const zbuf& buffer_;
    size_t offset_{};
};
} // namespace detail

// this is slow and stupid json DOM
class model : private parser<model>
{
    using parser_type = parser<model>;
    using object = detail::object;
    using model_view = detail::model_view;
    using jsonval = detail::jsonval;
    using array = detail::array;

    constexpr static size_t npos = std::numeric_limits<size_t>::max();

public:
    bool has_key(const char* key) const noexcept
    {
        return model_view{buf_, 0}.has_key(key);
    }

    model_view child(const char* key) const noexcept
    {
        return model_view{buf_, 0}.child(key);
    }

    model_view root() const noexcept
    {
        return model_view{buf_, 0};
    }

    void parse(const char* json_string)
    {
        assert(buf_.empty() && stack_.empty());
        parser_type::parse(json_string);
    }

private:
    void on_key(string_literal lit)
    {
        const auto offset = buf_.size();
        append_buf(object{0, lit});
        assert(offset + sizeof(jsonval) == buf_.size()); // in case zbuf changes and starts aligning data which would be a disaster
        assert(!stack_.empty()); // if this fires, then there is something wrong with the parser

        const auto prev_offset = stack_.top();
        if (prev_offset != npos)
        {
            const auto prev_obj = &buf_.at_offset<object>(prev_offset);
            prev_obj->next = offset; 
        }
        stack_.top() = offset;
    }

    void on_value(string_literal lit)
    {
        append_buf(lit);
    }

    void on_new_object()
    {
        stack_.push(npos);
    }

    void on_object_end()
    {
        stack_.pop();
    }

    void on_new_array()
    {
        const auto offset = buf_.size();
        append_buf(array{0});
        stack_.push(offset);
   }

    void on_array(string_literal lit)
    {
        append_buf(lit);

        const auto prev = stack_.top();
        const auto arr = &buf_.at_offset<array>(prev);

        const auto offset = buf_.size();
        arr->next = buf_.size();

        append_buf(array{0});
        stack_.top() = offset;
    }

    void on_array_end()
    {
        stack_.pop();
    }

    template <typename T>
    void append_buf(T&& t)
    {
        buf_.append<jsonval>().assign(t);
    }

    friend class parser<model>;

    static_stack<size_t, 64> stack_; // TODO: synchronize with the underlaying json
    haisu::zbuf buf_;
};

} // namespace json
} // namespace haisu

