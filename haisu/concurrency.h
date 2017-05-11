#pragma once
#include <mutex>
#include <iostream>

namespace haisu
{

template <typename T>
class synchronized
{
public:
	class proxy
	{
	public:
		explicit proxy(synchronized& s)
			: base_(s)
		{
			base_.lock();
		}

		~proxy()
		{
			base_.unlock();
		}

		T* operator ->()
		{
			return &base_.t_;
		}

		const T* operator ->() const
		{
			return &base_.t_;
		}

	private:
		synchronized& base_;
	};

	proxy operator ->()
	{
		return proxy(*this);
	}

	const proxy operator->() const
	{
		return proxy(*this);
	}

	template <typename ...Args>
	synchronized(Args&&... args) : t_(std::forward<Args>(args)...)
	{
	}

private:
	void lock()
	{
		mutex_.lock();
	}

	void unlock()
	{
		mutex_.unlock();
	}

	T t_;
	std::recursive_mutex mutex_;
};

} // namespace haisu

