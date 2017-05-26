#pragma once

#include <limits>
#include "meta.h"

namespace haisu
{
namespace mono
{

template <typename T, int N, typename Offset = meta::memory_requirement_t<N>>
class list
{
	using offset_type = Offset;
	enum {nil = std::numeric_limits<offset_type>::max()};
	static_assert(std::numeric_limits<offset_type>::max() >= N, "");

public:
	using size_type = offset_type;
	
	constexpr list() noexcept
	{
		clear_init();
	}

	list(const list&) = delete;
	list& operator =(const list&) = delete;
	list(const list&&) = delete;
	list& operator =(const list&&) = delete;

	list(std::initializer_list<T> ll)
	{
		*this = std::move(ll);
	}

	list& operator =(std::initializer_list<T> ll)
	{

		for (auto l: ll)
		{
			push_back(std::move(l));
		}

		return *this;
	}

	void clear()
	{
		// TODO: this does not call destructors
		if (_head != nil)
		{
			tail().next = _free_list;
			_free_list = _head;
			_head = _tail = nil;
		}	
	}

	void push_back(T t)
	{
		assert(!full());

		auto node = alloc();
		_buf[node].t = std::move(t);
		
		if (empty())
		{
			init(node);
		}
		else
		{
			_buf[node].next = nil;
			_buf[node].prev = _tail;
			
			tail().next = node;
			_tail = node;
		}
	}

	void push_front(T t)
	{
		assert(!full());
		auto node = alloc();
		_buf[node].t = std::move(t);

		if (empty())
		{
			init(node);
		}
		else
		{
			_buf[node].next = _head;
			_buf[node].prev = nil;

			head().prev = node;
			_head = node;
		}
	}

	T& back()
	{
		assert(!empty());
		return tail().t;
	}

	const T& back() const
	{
		assert(!empty());
		return tail().t;
	}

	T& front()
	{
		assert(!empty());
		return head().t;
	}

	const T& front() const
	{
		assert(!empty());
		return front().t;
	}

	bool empty() const
	{
		return (_head == nil);
	}

	bool full() const
	{
		return _free_list == nil;
	}

	size_type size() const
	{

		size_type res = 0;
		auto cur = _head;
		while (cur != _tail)
		{
			res += 1;
			cur = at(cur).next;	
		}	
		
		res += (_head == nil ? 0 : 1);
		return res;
	}

	size_type capacity() const
	{
		return N;
	}

	size_type max_size() const
	{
		return N;
	}

	T pop_back()
	{
		assert(!empty());
		auto n = _tail;
		if (tail().prev != nil)
		{
			_tail = tail().prev;
		}
		else
		{
			_tail = _head = nil;
		}

		free(n);
		return std::move(at(n).t);
	}

	T pop_front()
	{
		assert(!empty());
		auto n = _head;
		if (head().next != nil)
		{
			_head = head().next;
		}
		else
		{
			_tail = _head = nil;
		}

		free(n);
		return std::move(at(n).t);
	}


private:
	struct node
	{
		T t;
		offset_type prev;
		offset_type next;
	};

	void init(offset_type n)
	{
		_head = n;
		_tail = n;

		_buf[n].next = nil;
		_buf[n].prev = nil;
	}
	
	void clear_init()
	{
		for (int i = 0; i < N; ++i)
		{
			_buf[i].next = i + 1;
		}
		_free_list = 0;
		_buf[N - 1].next = nil;
		_head = _tail = nil;
	}

	node& tail()
	{
		return _buf[_tail];
	}

	node& head()
	{
		return _buf[_head];
	}

	node& at(offset_type index)
	{
		return _buf[index];
	}

	const node& tail() const
	{
		return _buf[_tail];
	}

	const node& head() const
	{
		return _buf[_head];
	}

	const node& at(offset_type index) const
	{
		return _buf[index];
	}

	offset_type alloc()
	{
		auto node = _free_list;
		_free_list = _buf[node].next;
		return node;
	}

	void free(offset_type n)
	{
		at(n).next = _free_list;
		_free_list = n;
	}

	offset_type _free_list = 0;
	offset_type _head = nil;
	offset_type _tail = nil; 
	node _buf[N];
};

static_assert(sizeof(list<int8_t, 2>) - sizeof(list<int8_t, 1>) == 3, "");
static_assert(sizeof(list<int8_t, 254>) - sizeof(list<int8_t, 253>) == 3, "");
static_assert(sizeof(list<int16_t, 256>) - sizeof(list<int16_t, 255>) == 6, "");
static_assert(sizeof(list<int16_t, 65534>) - sizeof(list<int16_t, 65533>) == 6, "");
static_assert(sizeof(list<int16_t, 65536>) - sizeof(list<int16_t, 65535>) == 12, "");

} // namespace mono
} // namespace haisu
