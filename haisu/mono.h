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

	bool operator <(const stack& other)
	{
		return 0 > memcmp(_stack, other._stack, sizeof(T) * capacity());
	}

	T top() const
	{
		assert(_size > 0 && _size < N);

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

} // namespace mono
} // namespace haisu

