#pragma once
#include <map>

#include "haisu/mono.h"
#include "haisu/tree.h"

namespace haisu
{
namespace metric
{

template <typename T>
struct measure
{
	T metric;
	double value;
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
		_tatal += other._total;
	}

	double avg() const
	{
		return _calls ? _total / _calls : 0;
	}

	double total() const
	{
		return _total;
	}

	int calls() const
	{
		return _calls;
	}

private:
	double _total;
	int _calls;
};

template <typename T, int N>
class table
{
public:
	void store(metric<T> val)
	{
	}

	void store(const stack& key, metric<T> val)
	{
	}

	tree<T, double> load() const
	{
		tree<T, double> res;
		for (auto i: data)
		{
			res[i->first()] = i->second.total();
		}
	}

private:

	using key_t = mono::overlow_stack<T, N>;
	using val_t = aggregate<T>;

	// TODO replace map with a faster container
	// we dont need to free memory, can use a custom memory allocator
	std::map<key_t, val_t> _data;
};

class timer
{
public:
	using usec_t = unsigned long long;

	void start()
	{
	}

	usec_t stop()
	{
		return elapsed();
	}

	usec_t elapsed()
	{
		return _stopped - _started;
	}

private:
	usec_t _started = 0;
	usec_t _stopped = 0;
};

template <typename T, int N>
class timer_stack
{
public:
	void push(T t)
	{
		timer t;
		if (!_stack.overflow())
		{
			t.start();
		}
		_stack.push(t);
	}

	double pop()
	{
	}

private:
	mono::overflow_stack<timer, N> _stack;
};

template <typename T, int N>
class monitor
{
public:
	void start(T id)
	{
	}

	void stop()
	{
	}
private:
};

} // namespace metrics

} // namespace haisu
