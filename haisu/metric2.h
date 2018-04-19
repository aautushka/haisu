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

namespace haisu
{
namespace metric2
{

// TODO:
// 1. limit stack depth
// 2. limit number of metrics stored
// 3. contiguos object pool
template <typename K, typename V>
class trie
{
public:
    using key_type = K;
    using value_type = V;

    ~trie()
    {
        foreach_node(root, [](auto n){ delete n; });
    }

    value_type& up()
    {
        assert(cursor != nullptr);
        auto& prev = cursor->value;
        cursor = cursor->get_parent();
        return prev;
    }

    value_type& down(key_type key)
    {
        if (cursor)
        {
            cursor = add_child(cursor, key);
        }
        else
        {
            if (root)
            {
                assert(key == root->key);
                cursor = root;
            }
            else 
            {
                root = cursor = new_node();
                root->key = key;
            }
        }
        return cursor->value;
    }

    value_type& get()
    {
        assert(cursor != nullptr);
        return cursor->value;
    }

    value_type& at(std::initializer_list<key_type>&& path)
    {
        node* res = nullptr;
        for (auto p : path)
        {
            res = get_child(res, p);
            assert(res);
        }
        return res->value;
    }

    bool has(std::initializer_list<key_type>&& path)
    {
        node* res = nullptr;
        for (auto p : path)
        {
            res = get_child(res, p);
            if (!res)
            {
                return false;
            }
        }
        return true;
    }

    value_type& create(std::initializer_list<key_type>&& path)
    {
        node* res = nullptr;
        for (auto p: path)
        {
            res = create_child(res, p);
        }

        return res->value;
    }

    void foreach(auto&& func)
    {
        foreach_node(root, [&func](auto node) { func(node->key, node->value); });
    }

private:
    struct node 
    {
        node* parent;
        node* child;
        node* sibling;
        key_type key;
        value_type value;

        node* get_parent() { return parent;} 
        node* get_child() { return child; }
        node* get_sibling() { return sibling; }
        void share_parent(node* other) { parent = other->parent; }

        node* find_child(key_type key)
        {
            auto child = get_child();
            while (child && child->key != key)
            {
                child = child->get_sibling();
            }

            return child;
        }

        // finds a child or, if not present, its insertion place
        // assumes there is at least one child
        node* guess_child(key_type key)
        {
            auto child = get_child();
            assert(child != null);
            while (child->key != key && child->get_sibling() != nullptr)
            {
                child = child->get_sibling();
            }

            return child;
        }
    };

    node* new_node()
    {
        return new node{};
    }

    node* get_child(node* parent, key_type key)
    {
        if (parent)
        {
            return parent->find_child(key);
        }
        else
        {
            if (root && key == root->key)
            {
                return root;
            }
        }

        return nullptr;
    }

    node* add_child(node* p, key_type key)
    {
        if (p->child)
        {
            p = p->guess_child(key);

            if (p->key != key)
            {
                auto sibling = new_node();

                sibling->share_parent(p);
                sibling->key = key;
                p->sibling = sibling;
                p = sibling;
            }
        }
        else
        {
            auto child = new_node();
            child->parent = p;
            child->key = key;
            p->child = child;
            p = child;
        }
        return p;
    }

    node* create_child(node* parent, key_type key)
    {
        if (!parent)
        {
            assert(root == nullptr);
            root = new_node();
            root->key = key;
            return root;
        }
        else
        {
            auto existing_child =  get_child(parent, key);
            return existing_child ? existing_child : add_child(parent, key);
        }
    }

    void foreach_node(node* p, auto&& func)
    {
        if (p)
        {
            foreach_node(p->get_child(), std::forward<decltype(func)>(func)); 
            foreach_node(p->get_sibling(), std::forward<decltype(func)>(func));

            std::forward<decltype(func)>(func)(p);
        }
    }

    node* cursor = nullptr;
    node* root = nullptr;
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
        trie_.up().stop();
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
