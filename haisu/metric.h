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

    static usec_t start(usec_t elapsed)
    {
        return now() - elapsed;
    }

    static usec_t stop(usec_t elapsed)
    {
        return now() - elapsed;
    }

private:
    static usec_t usec(timeval time)
    {
        return (usec_t)time.tv_sec * 1000 * 1000 + time.tv_usec;
    }

    usec_t _elapsed = 0;
};

class aggregate_timer
{
public:
    using usec_t = unsigned long long;

    void start()
    {
        _elapsed = now() - _elapsed;
    }

    usec_t stop()
    {
        ++_calls;
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
    unsigned long _calls = 0;
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
