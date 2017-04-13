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

	size_t refs() const
	{
		return _refs;
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

// a linked list of mmap objects
// T is a payload type, a memory managing structure
template <typename T>
class melist
{
public:
	melist()
	{
	}

	~melist()
	{
		while (!_list.empty())
		{
			node_t& node = _list.pop_front();
			node.~node();
		}
	}

	// TODO: align to page size
	T& push_back(size_t size)
	{
		auto node = create_node(size + overhead()); 
		_list.push_back(*node);
		return node->data.man;
	}

	// TODO: align to page size
	T& push_front(size_t size)
	{
		auto node = create_node(size + overhead()); 
		_list.push_front(*node);
		return node->data.man;
	}

	void erase(T& t)
	{
		node_t& node = reinterpret_cast<node_t&>(t);
		_list.erase(node);
		node.~node();
	}

	T& front()
	{
		return _list.front().data.man;
	}

	T& back()
	{
		return _list.back().data.man;
	}

	static constexpr size_t overhead()
	{
		return sizeof(node_t);
	}

	bool empty() const
	{
		return _list.empty();
	}

	size_t size() const
	{
		return _list.size();
	}

private:

	struct header
	{
		T man;	
		memap map;

		header() {}
		header(header&& other) : map(std::move(other.map)), man(std::move(other.man)) {} 
	};

	using list = intrusive_list<header>;
	using node_t = typename list::node;

	node_t* create_node(size_t size)
	{
		memap map;
		map.create(size);	

		uint8_t* ptr = static_cast<uint8_t*>(map.get());
		assert(overhead() < map.size());

		auto node = new(ptr) node_t;

		// TODO: alignment
		void* freemem = ptr + overhead();
		size_t freesize = map.size() - overhead(); 

		// TODO: what if T is an integral type
		node->data.man.assign(freemem, freesize);
		node->data.map = std::move(map);

		assert((void*)ptr == (void*)&node->data);
		assert((void*)ptr == (void*)node);
		
		return node;
	}

	list _list;	

};

// a mmap-based pointer-bump memory
// adds more mmap's when exhausted
// never free's individual allocations
// never recycles the used memory
// frees all memory all at once in .dtor
// has almost no memory overhead
// never uses heap
// TODO: implement memory alignment strategy
class growbump
{
	using list_t = melist<bufbump>;
	enum {BLOCK_SIZE = 4096 - list_t::overhead()};
public:
	growbump(growbump&) = delete;
	growbump operator =(growbump&) = delete;

	growbump() 
	{
		_list.push_back(BLOCK_SIZE);
	}

	void* alloc(size_t size)
	{
		void* res = back().alloc(size);
		if (nullptr == res)
		{
			if (size < BLOCK_SIZE)
			{
				// TODO: we might be wasting memory here when we prematurely push it to backlog
				_list.push_back(BLOCK_SIZE);
				return alloc(size);
			}
			else
			{
				auto& man = _list.push_front(size);
				return man.alloc(size);
			}
		}
	}

	template <typename T>
	T* alloc()
	{
		void* res = alloc(sizeof(T));
		return static_cast<T*>(res);
	}

	void free(void*)
	{
		// do not implement this methods
		// the class is not capable of freeing memory
		// it's optimized for memory usage and sheer performance
	}

private:

	bufbump& back()
	{
		return _list.back();
	}

	melist<bufbump> _list;
}; 

// a mmap-based pointer-bump memory
// capable of growing indefinitely
// reference-counts allocations
// immediately returns memory to the system once there are no more references
// each memory allocation is preceeded by a service data structure, 
// therefore the class has a significant memory overhead: sizeof(void*) for every allocation
// never uses heap-memory
// TODO: implement alignment strategy
class refbump
{
	using list_t = melist<bufbump>;
	enum {BLOCK_SIZE = 4096 - list_t::overhead() - /*overhead()*/ sizeof(void*)};

public:
	refbump(growbump&) = delete;
	refbump operator =(refbump&) = delete;

	refbump() 
	{
		_list.push_back(BLOCK_SIZE);
	}

	void* alloc(size_t size)
	{
		void* res = alloc_from(back(), size);

		if (nullptr == res)
		{
			if (size < BLOCK_SIZE)
			{
				// TODO: we might be wasting memory here when we prematurely push it to backlog
				_list.push_back(BLOCK_SIZE);
				return alloc(size);
			}
			else
			{
				auto& man = _list.push_front(size + overhead());
				return alloc_from(man, size);
			}
		}

		return res;
	}

	template <typename T>
	T* alloc()
	{
		void* res = alloc(sizeof(T));
		return static_cast<T*>(res);
	}

	void free(void* ptr)
	{
		void** head = static_cast<void**>(ptr) - 1;
		bufbump* mem = static_cast<bufbump*>(*head);

		mem->free(ptr);
		if (mem->refs() == 0)
		{
			_list.erase(*mem);
		}
	}

	// returns then number of mmap's allocated by this memory manager
	size_t arena_count() const
	{
		return _list.size();
	}

private:
	void* alloc_from(bufbump& man, size_t size)
	{
		void** res = static_cast<void**>(man.alloc(size + overhead()));
		if (res)
		{
			*res = &man;
			return ++res;
		}

		return nullptr;
	}

	static constexpr size_t overhead()
	{
		return sizeof(void*);
	}


	bufbump& back()
	{
		return _list.back();
	}

	list_t _list;
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


