#pragma once

#include <sys/mman.h>
#include "intrusive.h"

namespace haisu
{
// memory map class, a thin wrapper around mmap 
class memap
{
public:
	memap()
	{
	}

	~memap()
	{
		if (created())
		{
			destroy();
		}
	}

	memap(const memap&) = delete;
	memap& operator =(const memap&) = delete;

	memap(memap&& other)
	{
		*this = std::move(other);
	}

	memap& operator =(memap&& other)
	{
		_ptr = other._ptr;
		other._ptr = nullptr;

		_size = other._size;
		other._size = 0;

		return *this;
	}

	void* get()
	{
		return _ptr;
	}

	size_t size() const
	{
		return _size;
	}

	void create(size_t size_bytes)
	{
		assert(!created());
		_ptr = mmap(nullptr, size_bytes, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); 
		_size = size_bytes;
	}

	void destroy()
	{
		assert(created());
		const size_t size = _size;
		void* const ptr = _ptr;

		_ptr = nullptr;
		_size = 0;

		munmap(ptr, size);
	}

private:

	bool created() const
	{
		return _ptr != nullptr;
	}

	void* _ptr = nullptr;
	size_t _size = 0;
};
	
// pointer bump memory, created specificly for POD types
// not ctor/dtor are being called
// can be freed all at once, no individual free operations are supported
// does not use heap memory
//
// TODO: alignment
class bufbump
{
public:
	bufbump()
	{
	}

	explicit bufbump(void* ptr, size_t size)
	{
		assign(ptr, size);
	}

	bufbump(const bufbump&) = delete;
	bufbump& operator =(const bufbump&) = delete;

	bufbump(bufbump&& other)
	{
		*this = std::move(other);
	}

	bufbump& operator =(bufbump&& other)
	{
		_cursor = other._cursor;
		_size = other._size;
		_ptr = other._ptr;
		_refs = other._refs;

		other.reset();
		return *this;
	}

	template <typename T> T* alloc()
	{
		return static_cast<T*>(alloc(sizeof(T)));
	}
	
	void* alloc(size_t size)
	{
		void* res = nullptr;
		if (size <= left())
		{
			res = cursor();
			move_cursor(size);
			++_refs;
		}
		return res;
	}

	void free(void* ptr)
	{
		assert(owns(ptr));
		assert(_refs > 0);

		if (--_refs == 0)
		{
			_cursor = begin(); 
		}
	}

	void assign(void* ptr, size_t size)
	{
		_cursor = ptr;
		_size = size;
		_ptr = ptr;
		_refs = 0;
	}

private:
	void reset()
	{
		_cursor = nullptr;
		_size = 0;
		_ptr = nullptr;
		_refs = 0;
	}

	bool owns(void *ptr)
	{
		return ptr >= begin() && ptr < end();
	}

	size_t left()
	{
		return end() - cursor();
	}

	size_t occupied()
	{
		return cursor() - begin(); 
	}

	size_t size()
	{
		return _size; 
	}

	uint8_t* cursor()
	{
		return static_cast<uint8_t*>(_cursor);
	}

	uint8_t* begin()
	{
		return static_cast<uint8_t*>(_ptr);
	}

	uint8_t* end()
	{
		return begin() + size();
	}

	void move_cursor(int delta)
	{
		_cursor = cursor() + delta;
	}

	void* _ptr = nullptr;
	size_t _size = 0;
	int _refs = 0;
	void* _cursor = nullptr;
};

// a pointer-bump memory running on top of mmap
// once all memory is exhausted it stops allocating and returns nullptr
// never grows
class podbump
{
public:
	explicit podbump(size_t size = 4 * 1024)
	{
		_map.create(size);
		_bump.assign(_map.get(), _map.size());

	}

	~podbump()
	{
		_map.destroy();
	}

	void* alloc(size_t size)
	{
		return _bump.alloc(size);
	}

	void free(void* ptr)
	{
		_bump.free(ptr);
	}

	template <typename T>
	T* alloc()
	{
		return _bump.alloc<T>();
	}

private:
	memap _map;
	bufbump _bump;
};

// a mmap-based pointer-bump memory
// adds more mmap's when exhausted
class growbump
{
	enum {BLOCK_SIZE = 4096};
public:
	growbump()
	{
		_list.push_back(*create_node());
	}

	~growbump()
	{
		while (!_list.empty())
		{
			node_t& node = _list.pop_front();
			node.~node();
		}
	}

	void* alloc(size_t size)
	{
		void* res = _list.back().data.bump.alloc(size);
		if (nullptr == res)
		{
			if (size < BLOCK_SIZE - sizeof(node_t))
			{
				// TODO: we might be wasting memory here when we prematurely push it to backlog
				auto node = create_node(BLOCK_SIZE);
				_list.push_back(*node);
				return alloc(size);
			}
			else
			{
				auto node = create_node(size + sizeof(node_t)); 
				_list.push_front(*node);
				return node->data.bump.alloc(size);
			}
		}
	}

	template <typename T>
	T* alloc()
	{
		void* res = alloc(sizeof(T));
		return static_cast<T*>(res);
	}

	void free()
	{
		// TODO
	}

private:

	struct header
	{
		memap map;
		bufbump bump;

		header() {}
		header(header&& other) : map(std::move(other.map)), bump(std::move(other.bump)) {} 
	};

	using list = intrusive_list<header>;
	using node_t = list::node;


	node_t* create_node(size_t size = BLOCK_SIZE)
	{
		memap map;
		map.create(size);	

		void* ptr = map.get();
		assert(sizeof(list::node) < map.size());

		list::node* node = new(ptr) list::node;

		void* user_data = static_cast<uint8_t*>(ptr) + sizeof(list::node);
		size_t user_size = map.size() - sizeof(list::node);

		node->data.bump.assign(user_data, user_size);
		node->data.map = std::move(map);
		
		return node;
	}

	list _list;	

}; 

// std-style allocator needed for inteoperability with standard containers
template <typename T, typename mem_t>
class allocator
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template <typename U>
	struct rebind{ typedef allocator<U, mem_t> other; };

	allocator()
	{
	}

	explicit allocator(mem_t& mem)
		: _mem(&mem)
	{
	}

	explicit allocator(const allocator& other)
		: _mem(other._mem)
	{
	}

	template <typename U>
	allocator(const allocator<U, mem_t>& other)
		: _mem(other._mem)
	{
	}

	allocator& operator =(const allocator& other)
	{
		_mem = other._mem;
		return *this;
	}

	template <typename U>
	allocator& operator =(const allocator<U, mem_t>& other)
	{
		_mem = other._mem;
		return *this;
	}

	pointer address(reference ref) const
	{
		return &ref;
	}

	const_pointer address(const_reference ref) const
	{
		return &ref;
	}

	pointer allocate(size_type n)
	{
		assert(_mem != nullptr);
		return static_cast<pointer>(_mem->alloc(n * sizeof(value_type)));
	}

	void deallocate(pointer p, size_t size)
	{
		assert(_mem != nullptr);
		_mem->free(p);
	}

	size_type max_size() const
	{
		return -1;
	}

	void construct(pointer p, const_reference val)
	{
		new (p) value_type(val);
	}

	void destroy(pointer p)
	{
		p->~value_type();
	}

	bool operator ==(const allocator& other) const
	{
		return _mem == other._mem;
	}

	bool operator !=(const allocator& other) const
	{
		return _mem != other._mem;
	}
private:
	mem_t* _mem = nullptr;
};

} // namespace haisu


