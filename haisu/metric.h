#pragma once
#include <map>

#include <sys/time.h>
#include <sys/resource.h>

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
	int _calls = 0;
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
		_started = now();
	}

	usec_t stop()
	{
		_stopped = now();
		return elapsed();
	}

	usec_t elapsed()
	{
		return _stopped - _started;
	}

	static usec_t now()
	{
		// TODO: std::chrono
		struct rusage ru;
		if (0 == getrusage(RUSAGE_THREAD, &ru))
		{
			timeval& user = ru.ru_utime;
			timeval& sys = ru.ru_stime;
			return usec(user) + usec(sys);
		}

		return 0;
	}

private:

	static usec_t usec(timeval time)
	{
		return (usec_t)time.tv_sec * 1000000 + time.tv_usec;
	}

	usec_t _started = 0;
	usec_t _stopped = 0;
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

	tree<T, unsigned long long> report()
	{
		return _table.query();
	}

	std::string report_json()
	{
		auto data = report();
		return "";
	}
	
private:
	table<T, N> _table;
	timer_stack<T, N> _timers;
	mono::overflow_stack<T, N> _path;
};


} // namespace metrics

} // namespace haisu
