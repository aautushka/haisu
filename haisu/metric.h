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
		_tatal += other._total;
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
	unsigned long long _total;
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

	tree<T, unsigned long long> load() const
	{
		tree<T, unsigned long long> res;
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
	void push(T id)
	{
		timer timer;
		timer.start();
		_stack.push(t);
	}

	unsigned long long pop()
	{
		return _stack.top().stop();	
	}

private:
	mono::stack<timer, N> _stack;
};

template <typename T, int N>
class monitor
{
public:
	void start(T id)
	{
		if (!_stack.full())
		{
			_timers.push(id);
		}
		_path.push(id);
	}

	void stop()
	{
		if (!_stack.overflow())
		{
			auto time = _timers.pop();
			_table.store(_stack, {_path.top(), time});
		}
		_path.pop();
	}

	tree<T, unsigned long long> report()
	{
	}
private:
	table<T, N> _table;
	timer_stack<T, N> _timers;
	mono::overflow_stack<T, N> _stack;
};

template <typaname T>
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
			_mon.stop();
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
		this._mon = other._mon;

		other._mon = nullptr;
	}

private:
	metric(T id, monitor& mon)
		: _mon(&mon)
	{
		_mon.start(id);
	}

	monitor* _mon = nullptr;
};

} // namespace metrics

} // namespace haisu
