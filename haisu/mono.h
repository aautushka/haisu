#pragma once

#include <type_traits>
#include <limits>

#include "algo.h"

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


template <int N>
class string
{
public:
	using reference = char&;
	using const_reference = const char&;
	using size_type = size_t;

	constexpr string()
	{
		clear();
	}

	constexpr string(size_type count, char ch)
	{
		assign(count, ch);
	}

	constexpr string(const string& str)
	{
		assign(str);
	}

	template <int M>
	constexpr string(const string<M>& str)
	{
		assign(str._buf, str.size());
	}

	constexpr string& operator =(const string& str)
	{
		return assign(str);
	}

	template <int M>
	constexpr string(const string<M>& str, size_type pos, size_type count = npos)
	{
		assign(str, pos, count);
	}

	template <int M>
	constexpr string& operator =(const string<M>& str)
	{
		return assign(str._buf, str.size());
	}

	constexpr string& operator =(const char* str)
	{
		return assign(str);
	}

	constexpr string& operator =(char ch)
	{
		_buf[0] = ch;
		resize(1);
	}

	constexpr string(const char* str, size_type count)
	{
		assign(str, count);
	}

	constexpr string(const char* str)
	{
		assign(str);
	}

	constexpr string& assign(size_type count, char ch)
	{
		assert(count <= N);
		memset(_buf, ch, count);
		resize(count);

		return *this;
	}

	constexpr string& assign(const string& str)
	{
		memcpy(_buf, str._buf, capacity());
		return *this;
	}

	template <int M>
	constexpr string& assign(const string<M>& str)
	{
		assign(str._buf, str.size());
	}

	constexpr string& assign(const char* str, size_type count)
	{
		assert(count <= N);
		memcpy(_buf, str, count);
		resize(count);
		return *this;
	}

	template <int M>
	constexpr string& assign(const string<M>& str, size_t pos, size_t count = npos)
	{
		assert(pos < str.size());

		size_t sz = std::min(count, str.size() - pos);

		assert(sz <= N);
		assign(str._buf + pos, sz);
		
		return *this;
	}

	constexpr string& assign(const char* str)
	{
		return assign(str, strlen(str));
	}
	
	constexpr const char* c_str() const
	{
		return data();
	}
	
	constexpr const char* data() const
	{
		return _buf;
	}

	constexpr char* data() 
	{
		return _buf;
	}

	constexpr size_t size() const
	{
		return N - _buf[N];
	}

	constexpr size_t length() const
	{
		return size();
	}

	constexpr bool empty() const
	{
		return size() == 0;	
	}

	constexpr static size_t capacity()
	{
		return N;
	}

	constexpr void clear()
	{
		_buf[0] = 0;
		_buf[N] = N;
	}

	constexpr void resize(size_type count)
	{
		assert(count <= N);

		_buf[N] = N - count;
		_buf[count] = 0;
	}

	constexpr reference at(size_type pos) 
	{
		assert(pos < N);
		return _buf[pos];
	}

	constexpr const_reference at(size_t pos) const
	{
		assert(pos < N);
		return _buf[pos];
	}

	constexpr reference operator [](size_t pos)
	{
		return at(pos);
	}

	constexpr const_reference operator [](size_t pos) const
	{
		return at(pos);
	}

	constexpr reference front()
	{
		assert(size() > 0);
		return _buf[0];
	}

	constexpr const_reference front() const
	{
		assert(size() > 0);
		return _buf[0];
	}

	constexpr reference back()
	{
		assert(size() > 0);
		return _buf[size() - 1];
	}

	constexpr const_reference back() const
	{
		assert(size() > 0);
		return _buf[size() = 1];
	}

	constexpr static size_type max_size()
	{
		return capacity();
	}

	constexpr string& append(size_type count, char ch)
	{
		const size_type sz = size();

		assert(sz + count <= N);
		memset(_buf + sz, ch, count);
		resize(sz + count);
	}

	template <int M>
	constexpr string& append(const string<M>& str)
	{
		append(str._buf, str.size());
	}

	constexpr string& append(const char* str, size_type count)
	{
		const size_type sz = size();

		assert(sz + count <= N);
		memcpy(_buf + sz, str, count);
		resize(sz + count);
		return (*this);
	}

	constexpr string& append(const char* str)
	{
		return append(str, strlen(str));
	}

	constexpr size_type free_capacity() const
	{
		return capacity() - size();
	}

	constexpr string& operator +=(const string& str)
	{
		return append(str);
	}

	constexpr string& operator +=(char ch)
	{
		return append(1, ch);
	}

	constexpr void push_back(char ch)
	{
		append(1, ch);
	}

	constexpr void pop_back()
	{
		assert(!empty());
		resize(size() - 1);
	}

	template <int M>
	constexpr int compare(const string<M>& str) const
	{
		return strcmp(_buf, str._buf);
	}

	constexpr int compare(const char* str) const
	{
		return strcmp(_buf, str);
	}

	template <int M>
	constexpr bool operator ==(const string<M>& str) const
	{
		return 0 == compare(str);
	}

	template <int M>
	constexpr bool operator !=(const string<M>& str) const
	{
		return 0 != compare(str);
	}

	template <int M>
	constexpr bool operator <(const string<M>& str) const
	{
		return compare(str) < 0;
	}

	template <int M>
	constexpr bool operator >(const string<M>& str) const
	{
		return compare(str) > 0;
	}

	template <int M>
	constexpr bool operator <=(const string<M>& str) const
	{
		return compare(str) <= 0;
	}

	template <int M>
	constexpr bool operator >=(const string<M>& str) const
	{
		return compare(str) >= 0;
	}

	constexpr string& erase(size_type index = 0, size_type count = npos)
	{
		assert(index <= size());
		const size_type cur_size = size();
		const size_type erase_size = std::min(count, cur_size - index);	
		const size_type erase_end = index + erase_size;
		const size_type leftover = cur_size - erase_end;
		memmove(_buf + index, _buf + erase_end, leftover);
		resize(cur_size - erase_size);
		return *this;
	}

	constexpr string substr(size_type pos = 0, size_type count = npos) const
	{
		assert(pos <= size());
		
		return string(_buf + pos, std::min(count, size() - pos));		
	}

	size_type copy(char* dest, size_type count, size_type pos = 0) const
	{
		assert(pos <= size());

		const size_t sz = std::min(size() - pos, count);
		memcpy(dest, _buf + pos, sz); 

		return sz;
	}

	template <int M>
	size_type find(const string<M>& str, size_type pos = 0) const
	{
		return find(str._buf, pos);	
	}

	size_type find(const char* str, size_type pos, size_type count) const
	{
		assert(pos <= size());
		const void* found = memmem(_buf + pos, size() - pos, str, count);
		return found ? static_cast<const char*>(found) - _buf : npos;
	}

	size_type find(const char* str, size_type pos = 0) const
	{
		assert(pos <= size());
		const char* found = strstr(_buf + pos, str);
		return found ? found - _buf : npos;
	}

	size_type find(char ch, size_type pos = 0) const
	{
		assert(pos <= size());
		const char* found = strchr(_buf + pos, ch);
		return found ? found - _buf : npos;
	}

	string& insert(size_type index, size_type count, char ch)
	{
		assert(size() + count <= N);

		char* const rhs = _buf + index;
		memmove(rhs + count, rhs, size() - index);
		memset(rhs, ch, count);
		resize(size() + count);


		return *this;
	}

	string& insert(size_type index, const char* str)
	{
		return insert(index, str, strlen(str));
	}

	string& insert(size_type index, const char* str, size_type count)
	{
		assert(size() + count <= N);

		char* const rhs = _buf + index;
		memmove(rhs + count, rhs, size() - index);
		memcpy(rhs, str, count);
		resize(size() + count);

		return *this;
	}

	template <int M>
	string& insert(size_type index, const string<M>& str)
	{
		return insert(index, str.c_str(), str.size());
	}

	template <int M>
	string& insert(size_type index, const string<M>& str, size_type index_str, size_type count = npos)
	{
		assert(index_str <= str.size());

		const size_type max_insert_count = str.size() - index_str;
		const size_type effective_count = std::min(count, max_insert_count);

		return insert(index, str._buf + index_str, effective_count);
	}

	template <int M>
	string& replace(size_type pos, size_type count, const string<M>& str)
	{
		return replace(pos, count, str._buf, str.size());
	}

	string& replace(size_type pos, size_type count, const char* str, size_type count2)
	{
		assert(pos <= size());
		assert(pos + count <= size());
		assert(size() - count + count2 >= size());

		char* const insert_pos = _buf + pos;
		memmove(insert_pos + count2, insert_pos + count, size() - pos - count);
		memcpy(insert_pos, str, count2);

		resize(size() + count2 - count);
		
		return *this;
	}

	string& replace(size_type pos, size_type count, const char* str)
	{
		return replace(pos, count, str, strlen(str));
	}

	string& replace(size_type pos, size_type count, size_type count2, char ch)
	{
		assert(pos <= size());
		assert(pos + count <= size());
		assert(size() - count + count2 >= size());

		char* const insert_pos = _buf + pos;
		memmove(insert_pos + count2, insert_pos + count, size() - pos - count);
		memset(insert_pos, ch, count2);

		resize(size() + count2 - count);
		
		return *this;
	}

	template <int M>
	size_type rfind(const string<M>& str, size_type pos = npos) const
	{
		return rfind(str._buf, pos, str.size());
	}

	size_type rfind(const char* str, size_type pos, size_type count) const
	{
		if (count == 0)
		{
			return pos == npos ? size() : 0;
		}

		if (pos < size())
		{
			pos = pos + count - 1;
		}

		pos = std::min(pos, size());
		const void *found = algo::memrmem(_buf, pos, str, count); 
		return found ? static_cast<const char*>(found) - _buf : npos;

	}

	size_type rfind(const char* str, size_type pos = npos) const
	{
		return rfind(str, pos, strlen(str));
	}

	size_type rfind(char ch, size_type pos = npos) const
	{
		pos = std::min(pos, size());
		const void *found = memrchr(_buf, ch, pos); 
		return found ? static_cast<const char*>(found) - _buf : npos;
	}

	template <int M>
	size_type find_first_of(const string<M>& str, size_type pos = 0) const
	{
		return find_first_of(str._buf, pos, str.size());
	}

	size_type find_first_of(const char* str, size_type pos, size_type count) const
	{
		assert(pos <= size());
		const char* cur = _buf + pos;
		while (*cur)
		{
			if (memchr(str, *cur, count))
			{
				return cur - _buf;
			}
			++cur;
		}

		return npos;
	}

	size_type find_first_of(const char* str, size_type pos = 0) const
	{
		return find_first_of(str, pos, strlen(str));
	}

	size_type find_first_of(char ch, size_type pos = 0) const
	{
		return find(ch, pos);
	}

	template <int M>
	size_type find_first_not_of(const string<M>& str, size_type pos = 0) const
	{
		return find_first_not_of(str._buf, pos, str.size());
	}

	size_type find_first_not_of(const char* str, size_type pos, size_type count) const
	{
		assert(pos <= size());
		const char* cur = _buf + pos;
		while (*cur)
		{
			if (!memchr(str, *cur, count))
			{
				return cur - _buf;
			}
			++cur;
		}

		return npos;
	}

	size_type find_first_not_of(const char* str, size_type pos = 0) const
	{
		return find_first_not_of(str, pos, strlen(str));
	}

	size_type find_first_not_of(char ch, size_type pos = 0) const
	{
		assert(pos <= size());
		const char* cur = _buf + pos;
		while (*cur)
		{
			if (*cur != ch)
			{
				return cur - _buf;
			}
			++cur;
		}

		return npos;
	}

	template <int M>
	size_type find_last_of(const string<M>& str, size_type pos = npos) const
	{
		return find_last_of(str._buf, pos, str.size());
	}

	size_type find_last_of(const char* str, size_type pos, size_type count) const
	{
		// TODO: implement
		return npos;
	}

	size_type find_last_of(const char* str, size_type pos = npos) const
	{
		return find_last_of(str, pos, strlen(str));
	}

	size_type find_last_of(char ch, size_type pos = npos) const
	{
		return rfind(ch, pos);
	}

	template <int M>
	size_type find_last_not_of(const string<M>& str, size_type pos = npos) const
	{
		return find_last_not_of(str._buf, pos, str.size());
	}

	size_type find_last_not_of(const char* str, size_type pos, size_type count) const
	{
		// TODO: implement
		return npos;
	}

	size_type find_last_not_of(const char* str, size_type pos = npos) const
	{
		return find_last_not_of(str, pos, strlen(str));
	}

	size_type find_last_not_of(char ch, size_type pos = npos) const
	{
		// TODO: impelemnt
		return npos;
	}
	
	// TODO: iterators

	enum constants : size_type {
		npos = std::string::npos
	};

private:
	char _buf[N + 1];

	template <int M> friend class string;
};

static_assert(16 == sizeof(string<15>), "too big overhead");

template <int N>
std::ostream& operator <<(std::ostream& stream, const string<N>& str)
{
	return stream << str.c_str();
}

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
	
	list()
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

