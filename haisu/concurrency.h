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
			: base_(&s)
		{
			base_->lock();
		}

		proxy(const proxy&) = delete;
		proxy& operator =(const proxy&) = delete;
		proxy(proxy&& rhs)
		{
			base_ = rhs.base_;
			rhs.base_ = nullptr;
		}

		proxy& operator =(proxy&& rhs)
		{
			if (base_)
			{
				base_->unlock();
				base_ = nullptr;
			}

			base_ = rhs.base_;
			rhs.base_ = nullptr;
		}

		~proxy()
		{
			if (base_)
			{
				base_->unlock();
			}
		}

		T* operator ->()
		{
			assert(base_ != nullptr);
			return &base_->t_;
		}

		const T* operator ->() const
		{
			return &base_.t_;
		}

	private:
		synchronized* base_ = nullptr;
	};

	proxy operator ->()
	{
		return std::move(proxy(*this));
	}

	/*const proxy operator->() const
	{
		return proxy(*this);
	}*/

	template <typename ...Args>
	synchronized(Args&&... args) : t_(std::forward<Args>(args)...)
	{
	}

	void lock()
	{
		mutex_.lock();
	}

	void unlock()
	{
		mutex_.unlock();
	}

private:

	T t_;
	std::recursive_mutex mutex_;
};

} // namespace haisu

