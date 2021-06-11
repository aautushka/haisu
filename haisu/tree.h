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
#include <memory>
#include <map>
#include <sstream>

namespace haisu
{
template <typename Key, typename Val>
class tree
{
    using leaves_t = std::map<Key, std::unique_ptr<tree>>;    

public:
    using self_type = tree<Key, Val>;
    using value_type = Val;
    using key_type = Key;

    using iterator = typename leaves_t::iterator;
    using const_iterator = typename leaves_t::const_iterator;

    tree()
    {
    }

    tree(const tree& other) = delete;
    tree& operator =(const tree& other) = delete;

    tree(tree&& other)
    {
        *this = std::move(other);
    }

    tree& operator =(tree&& other)
    {
        _val = std::move(other._val);
        _leaves = std::move(other._leaves);
        return *this;
    }

    template <typename T>
    value_type& operator [](const T& key)
    {
        return child(key)->get();
    }

    template <typename T>
    const value_type& operator [](const T& key) const
    {
        return child(key)->get();
    }

    value_type& operator [](const key_type& key)
    {
        return child(key)->get();
    }

    const value_type& operator [](const key_type& key) const
    {
        return child(key)->get();
    }

    void insert(const key_type& key, const value_type& val)
    {
        child(key) = val;
    }

    bool empty() const
    {
        return _leaves.empty();
    }

    iterator begin() 
    {
        return iterator(_leaves.begin());
    }

    iterator end()
    {
        return iterator(_leaves.end());
    }

    const_iterator begin() const
    {
        return _leaves.begin();
    }

    const_iterator end() const
    {
        return _leaves.end();
    }

    template <typename Func>
    void foreach(Func func) const
    {
        for (auto i = begin(); i != end(); ++i)
        {
            func(i->first, i->second->get());    
            i->second->foreach(func);
        }
    }

    value_type& get()
    {
        return _val;
    }

    const value_type& get() const
    {
        return _val;
    }

    template <typename T> 
    size_t count(const T& key) const
    {
        return find(key) ? 1 : 0;
    }

    void erase(const key_type& key)
    {
        _leaves.erase(key);
    }

    template <typename T> 
    void erase(const T& key)
    {
        tree* child = this;
        tree* parent = this;
        key_type leaf;
        for (auto k: key)
        {
            if (child != parent)
            {
                parent = child;
            }

            auto i = parent->_leaves.find(k);
            if (i == parent->_leaves.end())
            {
                // can't find a node to remove
                return;
            }
            else
            {
                child = i->second.get();
                leaf = k;
            }
        }

        parent->erase(leaf);
    }

    void clear()
    {
        _leaves.clear();
        _val = value_type();
    }

    size_t size() const
    {
        size_t sum = 0;
        foreach([&](auto, auto) { ++sum; });
        return sum;
    }

private:
    // TODO: gls::not_null
    tree* child(const key_type& key)
    {
        auto i = _leaves.find(key);
        if (i != _leaves.end())
        {
            return i->second.get();
        }
        else
        {
            std::unique_ptr<self_type> res(new tree);
            auto ptr = res.get();
            _leaves[key] = std::move(res);
            return ptr;
        }
    }

    // TODO: gsl::not_null
    const tree* child(const key_type& key) const
    {
        auto i = _leaves.find(key);
        if (i != _leaves.end())
        {
            return i->second.get();
        }

        throw std::exception();
    }

    // TODO: gsl::not_null
    template <typename T>
    tree* child(const T& key)
    {
        self_type* tr = this;
        for (auto k : key) tr = tr->child(k);
        return tr;
    }

    // TODO: gls::not_null
    template <typename T>
    const tree* child(const T& key) const
    {
        const self_type* tr = this;
        for (auto& k : key) tr = tr->child(k);
        return tr;
    }

    template <typename T> 
    const tree* find(T& key) const
    {
        const self_type* tr = this;
        for (auto k: key)
        {
            auto i = tr->_leaves.find(k);
            if (i != tr->_leaves.end())
            {
                tr = i->second.get();
            }
            else
            {
                return nullptr;
            }
        }

        return tr;
    }

    self_type* find(const key_type& key) const
    {
        auto i = _leaves.find(key);
        return i == _leaves.end() ? nullptr : i->second.get();
    }

    value_type _val;
    leaves_t _leaves;    
}; 

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
T to_json(T t)
{
    return t;
}

template <typename U, typename V>
void to_json(const tree<U, V>& tr, std::ostream& stream, bool write_default_value = false)
{
    stream << '{';

    if (write_default_value)
    {
        stream << "\"#\": \"" << to_json(tr.get()) << '"';
    }

    for (auto i = tr.begin(); i != tr.end(); ++i)
    {
        if (write_default_value || i != tr.begin())
        {
            stream << ", ";
        }

        stream << '"' << i->first << "\":";
        
        auto& child = *i->second;
        if (child.begin() != child.end())
        {
            to_json(*i->second, stream, true);
        }
        else
        {
            stream << '"' << to_json(child.get()) << '"';
        }
    }
    stream << '}';
}


template <typename U, typename V>
std::string to_json(const tree<U, V>& tr)
{
    std::stringstream ret;
    to_json(tr, ret); 
    return std::move(ret.str());
}

} // namespace haisu

