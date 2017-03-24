#pragma once

#include <atomic>
#include <functional>
#include <type_traits>

namespace haisu
{

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type nil_value()
{
	return 0;
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type nil_value()
{
	return T();
}

// thread-safe fixed-length linear-probing open addressing hash table 
// can't grow, can't rehash
// designed to be as simple as possible and hopefully fast
template <typename Key, typename Value, int N, typename Hash = std::hash<Key>>
class linear_hash
{
public:
	linear_hash()
		: linear_hash(nil_value<Key>())
	{
	}

	explicit linear_hash(const Key& nilval)
		: _nil(nilval)
		, _size(0)
	{
		clear();
	}
	
	~linear_hash()
	{
	}
	
	Value& operator [](const Key& key)
	{
		return find(key)->value;
	}

	const Value& operator[](const Key& key) const
	{
		return find(key)->value;
	}

	void insert(const Key& key, const Value& value)
	{
		find(key)->value = value;
	}

	void insert(const Key& key, Value&& value)
	{
		find(key)->value = value;
	}

	void erase(const Key& key)
	{
		pair_t* const p = find(key);
		p->value = Value();
		p->key.store(_nil, std::memory_order_relaxed);
		--_size;
	}

	// not thread-safe, better not use it
	bool empty() const
	{
		return 0 == size();
	}

	// not thread-safe, better not use it
	size_t size() const
	{
		return _size;
	}

	void clear()
	{
		for (int i = 0; i < N; ++i)
		{
			_data[i].value = Value();
			_data[i].key = _nil;
		}
		_size = 0;
	}

private:
	struct pair_t
	{
		std::atomic<Key> key;
		Value value;
	};

	pair_t* find(const Key& key)
	{
		int hash = Hash()(key) % N;
		int i = hash;
		do
		{
			pair_t* p = &_data[i];
			i = (i + 1) % N;
			Key prev = p->key.load(std::memory_order_relaxed);
			if (prev != _nil && prev == key)
			{
				return p;
			}
			else if (prev != _nil)
			{
				continue;
			}

			if (p->key.compare_exchange_strong(prev, key, std::memory_order_relaxed))
			{
				++_size;
				return p;
			}
		}
		while (i != hash);

		// if we get here then the hash is full 
		// you should consider increasing the N parameter or look for a different data structue
		assert(false);
	}

	pair_t _data[N];
	const Key _nil;
	size_t _size;
};

// this is a thread local storage implemented on top of hash container
// the problem with the regular thread_local is it's lifespan
// the thread_local variable lives as long as the thread 
// if you want to be able to recreate the thread_local object you're in trouble
// the tls<N> class addresses the issue
// it has one major drawback: it's significantly slower, but sometimes that's okay
// the class is inspired by boost::thread_specific_ptr
template <typename T, int N = 131>
class tls
{
public:
	tls()
		: _t(nullptr)
	{
	}

	explicit tls(T* t)
		: _t(t)
	{
	}

	~tls()
	{
		delete _t;
	}
	
	tls(const tls&) = delete;
	tls& operator =(const tls&) = delete;

	T& operator *()
	{
		return *_t;
	}

	const T& operator *() const
	{
		return *_t;
	}

	T* operator ->()
	{
		return _t;
	}

	const T* operator ->() const
	{
		return _t;
	}

	T* release()
	{
		T* out = _t;
		_t = nullptr;
		return out;
	}

	void reset(T* t = nullptr)
	{
		delete _t;
		_t = t;
	}

private:
	T* _t;
};
} // namespace haisu 


