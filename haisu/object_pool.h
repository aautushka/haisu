#pragma once

namespace haisu
{

template <typename T, int N>
class object_pool
{
public:
	static_assert(N > 0, "empty pool is not allowed");
	object_pool()
	{
		create_free_list();
	}

	~object_pool()
	{
	}

	void destroy_all()
	{
	}

	T* alloc()
	{
		if (free_)
		{
			auto ret = &free_->obj;
			free_ = free_->next;
			return ret;
		}

		return nullptr;
	}

	template <typename ... Args>
	T* construct(Args&& ... args)
	{
		if (free_)
		{
			auto ret = &free_->obj;
			new (ret) T(std::forward<Args>(args)...); 
			free_ = free_->next;
			return ret;
		}

		return nullptr;
	}

	void dealloc(T* t)
	{
		assert(belongs(t));
		auto o = reinterpret_cast<object*>(t);
		o->next = free_;
		free_ = o;	
	}

	void destroy(T* t)
	{
		t->~T();
		dealloc(t);
	}

	bool belongs(const T* t) const
	{
		return t >= &pool_[0].obj && t <= &pool_[N - 1].obj;
	}

	static size_t capacity()
	{
		return N;
	}

	size_t size() const
	{
		auto o = free_;
		auto free_size = 0;
		while (o)
		{
			++free_size;
			o = o->next;
		}
		return capacity() - free_size;
	}

private:
	void create_free_list()
	{
		for (int i = 0; i < N - 1; ++i)
		{
			pool_[i].next = &pool_[i + 1];
		}
		pool_[N - 1].next = nullptr;
		free_ = &pool_[0];
	}

	union object
	{
		T obj;
		object* next;

		object() {}
		~object() {}
	};

	object* free_ = nullptr;
	object pool_[N];
};
	
} // namespace haisu


