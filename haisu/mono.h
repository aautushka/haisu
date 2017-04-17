#pragma once

#include <type_traits>
#include <limits>

namespace haisu
{
namespace mono
{

template <typename T, int N = 256>
class stack
{
public:
	static_assert(std::is_integral<T>::value, "integral type required");
	static_assert(N <= std::numeric_limits<T>::max(), "");

	class iterator
	{
	public:
		iterator() {} 
		explicit iterator(stack& parent) : _parent(&parent) { }
		T& operator *() {return _parent->_stack[_cursor]; }
		const T& operator *() const {return _parent->_stack[_cursor];}
		iterator& operator ++() { ++_cursor; return *this; }
		iterator operator ++(int) { iterator out(*this); ++(*this); return out;}
		T* operator ->() { return &**this; }
		const T* operator ->() const {return &**this;}

		bool operator !=(const iterator& other) const { return !(*this == other); }
		bool operator ==(const iterator& other) const 
		{
			if (_parent && other._parent)
			{
				assert(_parent == other._parent);
				return _cursor == other._cursor;
			}

			return end() && other.end();
		}

	private:
		bool end() const
		{
			return !_parent || _cursor < 0 || _cursor >= _parent->size();
		}

		stack* _parent = nullptr;
		int _cursor = 0;
	};

	class const_iterator
	{
	public:
		using self = const_iterator;

		const_iterator() {} 
		explicit const_iterator(const stack& parent) : _parent(&parent) { }
		const T& operator *() {return _parent->_stack[_cursor]; }
		const T& operator *() const {return _parent->_stack[_cursor];}
		self& operator ++() { ++_cursor; return *this; }
		self operator ++(int) { self out(*this); ++(*this); return out;}
		const T* operator ->() { return &**this; }
		const T* operator ->() const {return &**this;}

		bool operator !=(const self& other) const { return !(*this == other); }
		bool operator ==(const self& other) const 
		{
			if (_parent && other._parent)
			{
				assert(_parent == other._parent);
				return _cursor == other._cursor;
			}

			return end() && other.end();
		}

	private:
		bool end() const
		{
			return !_parent || _cursor < 0 || _cursor >= _parent->size();
		}

		const stack* _parent = nullptr;
		int _cursor = 0;
	};

	stack()
	{
		memset(_stack, 0, byte_capacity());
	}

	stack(const stack& other)
	{
		*this = other;
	}

	stack& operator =(const stack& other)
	{
		memcpy(_stack, other._stack, byte_capacity());
		_size = other.size();
		return *this;
	}

	stack(std::initializer_list<T> ll)
		: stack()
	{
		for (auto l : ll) push(l);
	}

	template <int U>
	bool operator ==(const stack<T, U>& other) const
	{
		if (_size == other._size)
		{
			return 0 == memcmp(_stack, other._stack, _size * sizeof(T));
		}
		return false;
	}

	bool operator ==(const stack& other) const
	{
		return 0 == memcmp(this, &other, byte_capacity());
	}

	template <int U>
	bool operator !=(const stack<T, U>& other) const
	{
		return !(*this == other);
	}

	void operator +=(T item)
	{
		push(item);
	}
	
	template <int U>
	void operator +=(const stack<T, U>& other)
	{
		assert(left() >= other.size());

		memcpy(_stack + _size, other._stack, sizeof(T) * other._size);
		_size += other._size;
	}

	bool operator <(const stack& other) const
	{
		return 0 > memcmp(_stack, other._stack, byte_capacity());
	}

	T top() const
	{
		assert(_size > 0 && _size <= N);

		return _stack[_size - 1];
	}

	void pop()
	{
		assert(_size > 0);

		_stack[--_size] = 0;
	}

	void push(T t)
	{
		assert(_size < N);
		_stack[_size++] = t;
	}

	iterator begin() 
	{
		return iterator(*this); 
	}

	iterator end() 
	{
		return iterator();
	}

	const_iterator cbegin() const
	{
		return const_iterator(*this);
	}

	const_iterator cend() const
	{
		return const_iterator();
	}

	const_iterator begin() const
	{
		return cbegin();
	}

	const_iterator end() const
	{
		return cend();
	}

	size_t size() const
	{
		return _size;
	}

	bool empty() const
	{
		return 0 == size();
	}

	static constexpr size_t capacity()
	{
		return N;
	}
	
	const T& front() const
	{
		assert(_size > 0);
		return _stack[0];
	}

	const T& back() const
	{
		assert(_size > 0);
		return _stack[_size - 1];
	}
	
	T& front()
	{
		assert(_size > 0);
		return _stack[0];
	}

	T& back()
	{
		assert(_size > 0);
		return _stack[_size - 1];
	}

	bool full() const
	{
		return _size == capacity();
	}

private:
	size_t left() const
	{
		return capacity() - _size;
	}

	static constexpr size_t byte_capacity()
	{
		return capacity() * sizeof(T);
	}

	T _size = 0;
	T _stack[N];

	template <typename U, int V> friend class stack;
};

static_assert(sizeof(stack<char, 15>) == 16, "the structure must be tightly packed"); 

// overflow-tolerant stack, ignores everything what goes beyond the boundary
template <typename T, int N = 256>
class overflow_stack
{
public:
	using self_type = overflow_stack;
	using iterator = typename stack<T, N>::iterator;
	using const_iterator = typename stack<T, N>::const_iterator;

	overflow_stack()
	{
	}

	overflow_stack(std::initializer_list<T> ll)
	{
		for (auto l : ll) push(l);
	}

	void push(T t)
	{
		if (_stack.size() < _stack.capacity())
		{
			_stack.push(t);
		}
		else
		{
			++_overflow;
		}
	}

	void pop()
	{
		if (overflow())
		{
			--_overflow;
		}
		else
		{
			_stack.pop();
		}
	}

	T top()
	{
		assert(_stack.size() > 0 && !overflow());
		return _stack.top();
	}
	
	size_t size() const
	{
		return _stack.size() + _overflow;
	}

	bool empty()
	{
		return 0 == size();
	}

	static constexpr size_t capacity()
	{
		return N;
	}

	T& front()
	{
		return _stack.front();
	}

	const T& front() const
	{
		return _stack.front();
	}

	T& back()
	{
		assert(!overflow());
		return _stack.back();
	}

	const T& back() const
	{
		assert(!overflow());
		return _stack.back();
	}

	bool overflow() const
	{
		return _overflow > 0;
	}

	bool full() const
	{
		return size() >= capacity();
	}
	
	template <int M>
	bool operator ==(const overflow_stack<T, M>& other) const
	{
		// completely ignore the overflow part here
		return _stack == other._stack;
	}

	template <int M>
	bool operator !=(const overflow_stack<T, M>& other) const
	{
		// completely ignore the overflow part here
		return _stack != other._stack;
	}

	template <int M>
	bool operator <(const overflow_stack<T, M>& other) const
	{
		// completely ignore the overflow part here
		return _stack < other._stack;
	}

	void operator +=(T t)
	{
		push(t);
	}

	template <int M>
	void operator +=(const overflow_stack<T, M>& other)
	{
		// TODO: not efficient
		for (auto i = other._stack.begin(); i != other._stack.end(); ++i)
		{
			push(*i);
		}

		_overflow += other._overflow;
	}

	iterator begin()
	{
		assert(!overflow());
		return _stack.begin();
	}

	iterator end()
	{
		return _stack.end();
	}

	const_iterator begin() const
	{
		assert(!overflow());
		return _stack.begin();
	}

	const_iterator end() const
	{
		return _stack.end();
	}

	const_iterator cbegin() const
	{
		return begin();
	}

	const_iterator cend() const
	{
		return end();
	}

private:
	stack<T, N> _stack;
	int _overflow = 0;

	template <typename U, int M> friend class overflow_stack;
};

template <typename T>
constexpr bool power_of_two(T t)
{
	return t && (!(t & (t - 1)));
}

// a fixed length list, can hold up to N elements
template <typename T, int N>
class queue
{
public:
	using self_type = queue<T, N>;

	static_assert(power_of_two(N), "suboptimal performance");

	bool empty() const
	{
		return count == 0;
	}
	
	size_t capacity() const
	{
		return N;
	}

	bool full() const
	{
		return size() == capacity();	
	}

	size_t size() const
	{
		return count;
	}

	const T& front() const
	{
		assert(!empty());
		return data[head];
	}

	const T& back() const
	{
		assert(!empty());
		return data[prev(tail)];
	}


	T& front()
	{
		return data[head];
	}

	T& back()
	{
		return data[prev(tail)];
	}

	void clear()
	{
		count = head = tail = 0;
	}

	void push_back(T t)
	{
		assert(!full());

		data[tail] = std::move(t);

		move_tail_forward();
	}

	void push_front(T t)
	{
		assert(!full());

		move_head_backward();

		data[head] = std::move(t);
	}


	T& emplace_back()
	{
		assert(!full());

		T& res = data[tail];
		
		move_tail_forward();
		return res;
	}

	T& emplace_front()
	{
		assert(!full());

		move_head_backward();

		return data[head];
	}

	T pop_back()
	{
		assert(!empty());

		move_tail_backward();

		return std::move(data[tail]);
	}

	T pop_front()
	{
		assert(!empty());

		T t  = std::move(data[head]);
		move_head_forward();

		return std::move(t);
	}

	class iterator
	{
	public:
		using list_t = queue<T, N>;

		iterator(){ }
		iterator(list_t& list) : list(&list), cur(list.head), left(list.size()) { }

		bool operator ==(const iterator& other) const 
		{
			return (list == other.list && cur == other.cur) 
				|| (list == nullptr && other.left == 0) 
				|| (other.list == nullptr && left == 0); 
		}

		bool operator !=(const iterator& other) const { return !(*this == other); }
		T& operator *() { return list->data[cur]; } 
		const T& operator *() const { return list->data[cur]; }
		iterator& operator ++() { cur = list->next(cur); --left; return *this; }
		iterator operator ++(int) { iterator prev(*this); ++(*this); return prev; }
		T* operator ->() { &**this; }
		const T* operator ->() const { &**this; }

	private:
		list_t* list = nullptr;
		int cur = 0;
		int left = 0;
	};

	iterator begin() 
	{
		return iterator(*this);
	}

	iterator end()
	{
		return iterator();
	}
private:
	void move_tail_forward()
	{
		count += 1;
		move_forward(tail);
	}

	void move_head_backward()
	{
		count += 1;
		move_backward(head);
	}

	void move_tail_backward()
	{
		count -= 1;
		move_backward(tail);
	}

	void move_head_forward()
	{
		count -= 1;
		move_forward(head);
	}

	void move_backward(int& ptr)
	{
		ptr = ((N - 1) + ptr) % N;
	}

	void move_forward(int& ptr)
	{
		ptr = (ptr + 1) % N;
	}

	int prev(int ptr)
	{
		move_backward(ptr);
		return ptr;
	}

	int next(int ptr)
	{
		move_forward(ptr);
		return ptr;
	}

	int head = 0;
	int tail = 0;
	int count = 0;
	T data[N];
};


} // namespace mono
} // namespace haisu

