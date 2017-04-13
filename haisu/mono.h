#pragma once

#include <type_traits>

namespace haisu
{
namespace mono
{

template <typename T, int N = 256>
class stack
{
public:
	static_assert(std::is_integral<T>::value, "integral type required");

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
			return 0 == memcmp(_stack, other._stack, byte_capacity());
		}
		return false;
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
		return 0 > memcmp(_stack, other._stack, sizeof(T) * capacity());
	}

	T top() const
	{
		assert(_size > 0 && _size <= N);

		return _stack[_size - 1];
	}

	void pop()
	{
		assert(_size > 0);

		_stack[_size--] = 0;
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

private:
	size_t left() const
	{
		return capacity() - _size;
	}

	static constexpr size_t byte_capacity()
	{
		return capacity() * sizeof(T);
	}

	T _stack[N];
	int _size = 0;

	template <typename U, int V> friend class stack;
};

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
	
	size_t size()
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

private:
	stack<T, N> _stack;
	int _overflow = 0;

	template <typename U, int M> friend class overflow_stack;
};

} // namespace mono
} // namespace haisu

