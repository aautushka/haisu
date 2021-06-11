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

#pragma once
#include "intrusive.h"
#include "mono_list.h"

namespace haisu
{

namespace duo
{
template <typename T, int N>
class list
{
public:
    using size_type = size_t;
    using self_type = list<T, N>;

    list()
    {
    }

    list(std::initializer_list<T> ll)
    {
        for (auto l : ll) push_back(l);
    }

    ~list()
    {
        clear();
    }

    list(const self_type&) = delete;
    self_type& operator =(const self_type&) = delete;

    bool empty() const
    {
        return list_.empty();
    }

    size_type size() const
    {
        size_type ret = 0;
        for (auto& i: list_)
        {
            ret += i.size();
        }
        return ret;
    }

    T& front()
    {
        assert(!empty());
        return list_.front().data.front();
    }

    T& back()
    {
        assert(!empty());
        return list_.back().data.back();
    }

    const T& front() const
    {
        assert(!empty());
        return  list_.front().data.front();
    }

    const T& back() const
    {
        assert(!empty());
        return list_.back().data.back();
    }

    void clear()
    {
        while (!empty())
        {
            node_type& node = list_.pop_back();
            delete &node;
        }
    }

    void push_back(T t)
    {
        if (!list_.empty())
        {
            auto& last_node = list_.back();
            if (!last_node.data.full())
            {
                last_node.data.push_back(std::move(t));
                return;
            }
        }

        node_type* node = new node_type;
        list_.push_back(*node);
        node->data.push_back(std::move(t));
    }

    void push_front(T t)
    {
        if (!list_.empty())
        {
            auto& first_node = list_.front();
            if (!first_node.data.full())
            {
                first_node.data.push_front(std::move(t));
                return;
            }
        }

        node_type* node = new node_type;
        list_.push_front(*node);
        node->data.push_front(std::move(t)); 
    }

    T pop_back()
    {
        assert(!empty());
        auto& last_node = list_.back();
        T ret = last_node.data.pop_back();
        if (last_node.data.empty())
        {
            node_type& node = list_.pop_back();
            delete &node;
        }
        return std::move(ret);
    }

    T pop_front()
    {
        assert(!empty());
        auto& first_node = list_.front();
        T ret = first_node.data.pop_front();
        if (first_node.data.empty())
        {
            node_type& node = list_.pop_front();
            delete &node;
        }
        return std::move(ret);
    }

    void push_back(self_type&& tt)
    {
        list_.push_back(std::move(tt.list_));
    }

    T& emplace_back()
    {
        push_back(T());
        return back();
    }

    T& emplace_front()
    {
        push_front(T());
        return front();
    }

    // TODO: iterators

private:
    using mono_list = mono::list<T, N>;
    using list_type = intrusive_list<mono_list>;
    using node_type = typename list_type::node;
    list_type list_;
};
} // namspace duo
} // namespace haisu


