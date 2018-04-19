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
#include "haisu/metric.h"
#include "haisu/object_pool.h"
#include "haisu/meta.h"

namespace haisu
{
namespace metric2
{

// TODO:
// 1. limit stack depth
// 2. limit number of metrics stored
// 3. contiguos object pool
template <typename K, typename V, int N = 254>
class trie
{
public:
    using key_type = K;
    using value_type = V;

    ~trie()
    {
        /* foreach_node(root, [](auto n){ delete n; }); */
    }

    value_type& up()
    {
        assert(cursor != nullidx);
        auto& res = at(cursor).value;
        cursor = at(cursor).parent;
        return res;
    }

    void up(auto&& value_func)
    {
        assert(cursor != nullidx);
        at(cursor).value = value_func(at(cursor).value);
        cursor = at(cursor).parent;
    }

    value_type& down(key_type key)
    {
        if (cursor != nullidx)
        {
            cursor = add_child(cursor, key);
        }
        else
        {
            if (root != nullidx)
            {
                assert(key == at(root).key);
                cursor = root;
            }
            else 
            {
                root = cursor = new_node(key);
            }
        }

        return at(cursor).value;
    }

    void down(key_type key, auto&& value_func)
    {
        down(key);
        at(cursor).value = value_func(at(cursor).value);
    }

    value_type& get()
    {
        assert(cursor != nullidx);
        return at(cursor).value;
    }

    value_type& at(std::initializer_list<key_type>&& path)
    {
        auto res = nullidx;
        for (auto p : path)
        {
            res = get_child(res, p);
            assert(res != nullidx);
        }
        return at(res).value;
    }

    bool has(std::initializer_list<key_type>&& path)
    {
        auto res = nullidx;
        for (auto p : path)
        {
            res = get_child(res, p);
            if (res == nullidx)
            {
                return false;
            }
        }
        return true;
    }

    value_type& create(std::initializer_list<key_type>&& path)
    {
        auto res = nullidx;
        for (auto p: path)
        {
            res = create_child(res, p);
        }

        return at(res).value;
    }

    void foreach(auto&& func)
    {
        foreach_node(root, [&func, this](auto node) { func(at(node).key, at(node).value); });
    }

private:
    struct node
    {
        node* parent;
        node* child;
        node* sibling;
        key_type key;
        value_type value;
    };

    using index_type = node*;
    constexpr static index_type nullidx = nullptr;

    node& at(index_type idx)
    {
        assert(idx != nullidx);
        return *pool.at(idx);
    }

    index_type new_node()
    {
        return new_node({});
    }

    index_type new_node(key_type key, index_type parent = nullidx)
    {
        const auto index = pool.construct();
        auto& node = at(index);
        node.child = node.sibling = nullidx;
        node.parent = parent;
        node.key = key;
        return index;
    }

    index_type get_child(index_type parent, key_type key)
    {
        if (parent != nullidx)
        {
            return find_child(parent, key);
        }
        else
        {
            if (root != nullidx && key == at(root).key)
            {
                return root;
            }
        }

        return nullidx;
    }

    index_type add_child(index_type p, key_type key)
    {
        if (at(p).child != nullidx)
        {
            p = guess_child(p, key);

            if (at(p).key != key)
            {
                auto sibling = new_node(key, at(p).parent);
                at(p).sibling = sibling; // p may relocate
                p = sibling;
            }
        }
        else
        {
            auto child = new_node(key, p);
            at(p).child = child; // p may relocate
            p = child;
        }
        return p;
    }

    index_type create_child(index_type parent, key_type key)
    {
        if (parent == nullidx)
        {
            assert(root == nullidx);
            root = new_node();
            at(root).key = key;
            return root;
        }
        else
        {
            auto existing_child =  get_child(parent, key);
            return existing_child != nullidx ? existing_child : add_child(parent, key);
        }
    }

    void foreach_node(index_type p, auto&& func)
    {
        if (p != nullidx)
        {
            foreach_node(at(p).child, std::forward<decltype(func)>(func)); 
            foreach_node(at(p).sibling, std::forward<decltype(func)>(func));

            std::forward<decltype(func)>(func)(p);
        }
    }

    index_type find_child(index_type n, key_type key)
    {
        auto child = at(n).child;
        while (child != nullidx && at(child).key != key)
        {
            child = at(child).sibling;
        }

        return child;
    }

    // finds a child or, if not present, its insertion place
    // assumes there is at least one child
    index_type guess_child(index_type n, key_type key)
    {
        auto child = at(n).child;
        assert(child != nullidx);

        while (at(child).key != key && at(child).sibling != nullidx)
        {
            child = at(child).sibling;
        }

        return child;
    }

    index_type cursor = nullidx;
    index_type root = nullidx;
    haisu::heap_pool<node> pool;
};

template <typename T>
class monitor
{
public:
    class metric
    {
    public:
        metric()
        {
        }

        ~metric()
        {
            stop();
        }

        void stop()
        {
            if (_mon)
            {
                _mon->stop();
            }
            _mon = nullptr;
        }

        metric(const metric&) = delete;
        metric& operator =(const metric&) = delete;


        metric(metric&& other)
        {
            *this = std::move(other);
        }

        metric& operator =(metric&& other)
        {
            stop();
            _mon = other._mon;

            other._mon = nullptr;
            return *this;
        }

    private:
        metric(T id, monitor& mon)
            : _mon(&mon)
        {
            _mon->start(id);
        }

        monitor* _mon = nullptr;

        friend class monitor;
    };
    
    using report_t = tree<T, unsigned long long>;

    void start(T id)
    {
        trie_.down(id).start();
    }

    void stop()
    {
        trie_.up().start();
    }

    metric scope(T id)
    {
        return metric(id, *this);
    }

    metric operator ()(T id)
    {
        return scope(id);
    }

    report_t report()
    {
        report_t res;
        trie_.foreach([&res](auto key, auto& val)
        {
            res[{key}] = val.elapsed();
        });

        return res;
    }

    std::string report_json()
    {
        auto data = report();
        return haisu::to_json(report());
    }
    
private:
    using timer = haisu::metric::aggregate_timer;
    trie<T, timer> trie_;
};

} // namespace metric2

} // namespace haisu
