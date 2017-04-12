#pragma once

#include <sys/mman.h>

namespace haisu
{
namespace mem
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
		munmap(_ptr, _size);
		_ptr = nullptr;
		_size = 0;
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
class podbump
{
public:
	explicit podbump(size_t size = 1024)
	{
		_map.create(size);
		_cursor = _map.get();
	}

	~podbump()
	{
		_map.destroy();
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
			_cursor = _map.get();
		}
	}

private:
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
		return _map.size();
	}

	uint8_t* cursor()
	{
		return static_cast<uint8_t*>(_cursor);
	}

	uint8_t* begin()
	{
		return static_cast<uint8_t*>(_map.get());
	}

	uint8_t* end()
	{
		return static_cast<uint8_t*>(_map.get()) + size();
	}

	void move_cursor(int delta)
	{
		_cursor = cursor() + delta;
	}

	memap _map;
	int _refs = 0;
	void* _cursor = nullptr;
};

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

} // namespace mem
} // namespace haisu


