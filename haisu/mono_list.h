#pragma once

#include <limits>

namespace haisu
{
namespace mono
{

template <int N>
struct memory_requirement_bytes
{
	using type = uint32_t;
};

template <>
struct memory_requirement_bytes<1>
{
	using type = uint8_t;
};

template <>
struct memory_requirement_bytes<2>
{
	using type = uint16_t;
};

template <int N>
struct calc_memory
{
	enum { result = (N < 255 ? 1 : (N < 65535 ? 2 : 4)) };
};

template <int N>
struct memory_requirement
{
	using type = typename memory_requirement_bytes<calc_memory<N>::result>::type;
};

template <typename T, int N>
class list
{
	using ptr_t = typename memory_requirement<N>::type;
	enum {nil = std::numeric_limits<ptr_t>::max()};
public:
	using size_type = ptr_t;
	
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
		ptr_t prev;
		ptr_t next;
	};

	void init(ptr_t n)
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
		_free_list = _buf[node].next;
		return node;
	}

	void free(ptr_t n)
	{
		at(n).next = _free_list;
		_free_list = n;
	}

	ptr_t _free_list = 0;
	ptr_t _head = nil;
	ptr_t _tail = nil; 
	node _buf[N];
};

static_assert(sizeof(list<int8_t, 2>) - sizeof(list<int8_t, 1>) == 3, "");
static_assert(sizeof(list<int8_t, 254>) - sizeof(list<int8_t, 253>) == 3, "");
static_assert(sizeof(list<int16_t, 256>) - sizeof(list<int16_t, 255>) == 6, "");
static_assert(sizeof(list<int16_t, 65534>) - sizeof(list<int16_t, 65533>) == 6, "");
static_assert(sizeof(list<int16_t, 65536>) - sizeof(list<int16_t, 65535>) == 12, "");

} // namespace mono
} // namespace haisu
