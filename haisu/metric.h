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
#include <map>

#include <sys/time.h>
#include <sys/resource.h>

#include "haisu/mono_stack.h"
#include "haisu/tree.h"

namespace haisu
{
namespace metric
{

template <typename T>
struct measure
{
    T metric;
    unsigned long long value;
};

template <typename T>
class aggregate
{
public:
    void operator ()(measure<T> m)
    {
        ++_calls;
    }

    void operator ()(const aggregate& other)
    {
        _calls += other._calls;
        _total += other._total;
    }

    void add (measure<T> val)
    {
        ++_calls;
        _total += val.value;
    }

    double avg() const
    {
        return _calls ? static_cast<double>(_total) / _calls : 0;
    }

    unsigned long long total() const
    {
        return _total;
    }

    int calls() const
    {
        return _calls;
    }

private:
    unsigned long long _total = 0;
    unsigned long _calls = 0;
};

template <typename T, int N>
class table
{
public:
    using key_type = mono::overflow_stack<T, N>;
    
    void store(measure<T> val)
    {
        const key_type key{val.metric};
        _data[key].add(val);
    }

    void store(const key_type& key, measure<T> val)
    {
        _data[key].add(val);
    }

    tree<T, unsigned long long> query() const
    {
        tree<T, unsigned long long> res;
        for (auto& i: _data)
        {
            res[i.first] = i.second.total();
        }

        return res;
    }

private:

    using val_t = aggregate<T>;

    // TODO replace map with a faster container
    // we dont need to free memory, can use a custom memory allocator
    std::map<key_type, val_t> _data;
};

class timer
{
public:
    using usec_t = unsigned long long;

    void start()
    {
        _elapsed = now() - _elapsed;
    }

    usec_t stop()
    {
        _elapsed = now() - _elapsed;
        return elapsed();
    }

    usec_t elapsed()
    {
        return _elapsed;
    }

    static usec_t now()
    {
        // TODO: std::chrono? performance!
        //      benchmark gettimeofday -- it's supposed to be fast and work in user-space
        //      consider TSC
        timeval time;
        gettimeofday(&time, nullptr);
        return usec(time);
    }

private:

    static usec_t usec(timeval time)
    {
        return (usec_t)time.tv_sec * 1000 * 1000 + time.tv_usec;
    }

    usec_t _elapsed = 0;
};

template <typename T, int N>
class timer_stack
{
public:
    using usec_t = timer::usec_t;

    void push(T id)
    {
        _stack.push(timer::now());
    }

    usec_t pop()
    {
        auto res = timer::now() - _stack.top();
        _stack.pop();
        return res;
    }

private:
    mono::stack<usec_t, N> _stack;
};

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
        foreach_node([](auto n){ delete n; });
    }

    value_type& up()
    {
        assert(cursor != nullptr);
        auto& prev = cursor->value;
        cursor = cursor->parent;
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

private:
    struct node 
    {
        key_type key;
        value_type value;
        node* parent;
        node* child;
        node* sibling;
    };

    node* new_node()
    {
        return new node{};
    }

    node* get_child(node* parent, key_type key)
    {
        if (parent)
        {
            auto child = parent->child;
            while (child && child->key != key)
            {
                child = child->sibling;
            }

            return child;
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
        if (!p->child)
        {
            auto child = new_node();
            child->parent = p;
            child->key = key;
            p->child = child;
            p = child;
        }
        else
        {
            p = p->child;
            while (p->key != key && p->sibling != nullptr)
            {
                p = p->sibling;
            }

            if (p->key != key)
            {
                auto sibling = new_node();

                sibling->parent = p->parent;
                sibling->key = key;
                p->sibling = sibling;
                p = sibling;
            }
        }

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

    void foreach_node(auto&& func)
    {
    }

    node* cursor = nullptr;
    node* root = nullptr;
};

template <typename T, int N>
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
        if (!_path.full())
        {
            _timers.push(id);
        }
        _path.push(id);
    }

    void stop()
    {
        if (!_path.overflow())
        {
            auto time = _timers.pop();
            _table.store(_path, {_path.top(), time});
        }
        _path.pop();
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
        return _table.query();
    }

    std::string report_json()
    {
        auto data = report();
        return haisu::to_json(report());
    }
    
private:
    table<T, N> _table;
    timer_stack<T, N> _timers;
    mono::overflow_stack<T, N> _path;
};


} // namespace metrics

} // namespace haisu
