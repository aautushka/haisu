#pragma once

#include "mono_list.h"

namespace haisu
{
namespace mono
{

template <typename T, int N>
class xorlist
{
	using ptr_t = typename memory_requirement<N>::type;
	enum {nil = std::numeric_limits<ptr_t>::max()};
public:
	using size_type = ptr_t;
	
	xorlist()
	{
		clear_init();
	}

	xorlist(const xorlist&) = delete;
	xorlist& operator =(const xorlist&) = delete;
	xorlist(const xorlist&&) = delete;
	xorlist& operator =(const xorlist&&) = delete;

	xorlist(std::initializer_list<T> ll)
	{
		*this = std::move(ll);
	}

	xorlist& operator =(std::initializer_list<T> ll)
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
			tail().link = _free_list;
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
			_buf[node].link = _tail;
			tail().link ^= node;
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
			_buf[node].link = _head;
			head().link ^= node;
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
		ptr_t cur = _head;
		ptr_t prev = 0;
		ptr_t next = cur;
		while (cur != _tail)
		{
			res += 1;
			next = at(cur).link ^ prev;
			prev = cur;
			cur = next;
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
		if (tail().link != 0)
		{
			const auto new_tail = tail().link;
			at(new_tail).link ^= _tail;
			_tail = new_tail;
			
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
		if (head().link != 0)
		{
			const auto new_head = head().link;
			at(new_head).link ^= _head;
			_head = new_head;
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
		ptr_t link;
	};

	void init(ptr_t n)
	{
		_head = n;
		_tail = n;

		_buf[n].link = 0;
	}
	
	void clear_init()
	{
		for (int i = 0; i < N; ++i)
		{
			_buf[i].link = i + 1;
		}
		_free_list = 0;
		_buf[N - 1].link = nil;
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

	node& at(ptr_t index)
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

	const node& at(ptr_t index) const
	{
		return _buf[index];
	}

	ptr_t alloc()
	{
		auto node = _free_list;
		_free_list = _buf[node].link;
		return node;
	}

	void free(ptr_t n)
	{
		at(n).link = _free_list;
		_free_list = n;
	}

	ptr_t _free_list = 0;
	ptr_t _head = nil;
	ptr_t _tail = nil; 
	node _buf[N];
};

static_assert(sizeof(xorlist<int8_t, 2>) - sizeof(xorlist<int8_t, 1>) == 2, "");
static_assert(sizeof(xorlist<int8_t, 254>) - sizeof(xorlist<int8_t, 253>) == 2, "");
static_assert(sizeof(xorlist<int16_t, 256>) - sizeof(xorlist<int16_t, 255>) == 4, "");
static_assert(sizeof(xorlist<int16_t, 65534>) - sizeof(xorlist<int16_t, 65533>) == 4, "");
static_assert(sizeof(xorlist<int16_t, 65536>) - sizeof(xorlist<int16_t, 65535>) == 8, "");

} // namespace mono
} // namespace haisu
