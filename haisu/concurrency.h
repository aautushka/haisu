/*
MIT License

Copyright (c) 2017 Anton Autushka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
			return *this;
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
			return &base_->t_;
		}

	private:
		synchronized* base_ = nullptr;
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

