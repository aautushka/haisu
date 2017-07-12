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
#include "intrusive.h"
#include "mono_list.h"

namespace haisu
{

namespace duo
{
template <typename T, int N>
class list
{
public:
	using size_type = size_t;
	using self_type = list<T, N>;

	list()
	{
	}

	list(std::initializer_list<T> ll)
	{
		for (auto l : ll) push_back(l);
	}

	~list()
	{
		clear();
	}

	list(const self_type&) = delete;
	self_type& operator =(const self_type&) = delete;

	bool empty() const
	{
		return list_.empty();
	}

	size_type size() const
	{
		size_type ret = 0;
		for (auto& i: list_)
		{
			ret += i.size();
		}
		return ret;
	}

	T& front()
	{
		assert(!empty());
		return list_.front().data.front();
	}

	T& back()
	{
		assert(!empty());
		return list_.back().data.back();
	}

	const T& front() const
	{
		assert(!empty());
		return  list_.front().data.front();
	}

	const T& back() const
	{
		assert(!empty());
		return list_.back().data.back();
	}

	void clear()
	{
		while (!empty())
		{
			node_type& node = list_.pop_back();
			delete &node;
		}
	}

	void push_back(T t)
	{
		if (!list_.empty())
		{
			auto& last_node = list_.back();
			if (!last_node.data.full())
			{
				last_node.data.push_back(std::move(t));
				return;
			}
		}

		node_type* node = new node_type;
		list_.push_back(*node);
		node->data.push_back(std::move(t));
	}

	void push_front(T t)
	{
		if (!list_.empty())
		{
			auto& first_node = list_.front();
			if (!first_node.data.full())
			{
				first_node.data.push_front(std::move(t));
				return;
			}
		}

		node_type* node = new node_type;
		list_.push_front(*node);
		node->data.push_front(std::move(t)); 
	}

	T pop_back()
	{
		assert(!empty());
		auto& last_node = list_.back();
		T ret = last_node.data.pop_back();
		if (last_node.data.empty())
		{
			node_type& node = list_.pop_back();
			delete &node;
		}
		return std::move(ret);
	}

	T pop_front()
	{
		assert(!empty());
		auto& first_node = list_.front();
		T ret = first_node.data.pop_front();
		if (first_node.data.empty())
		{
			node_type& node = list_.pop_front();
			delete &node;
		}
		return std::move(ret);
	}

	void push_back(self_type&& tt)
	{
		list_.push_back(std::move(tt.list_));
	}

	T& emplace_back()
	{
		push_back(T());
		return back();
	}

	T& emplace_front()
	{
		push_front(T());
		return front();
	}

	// TODO: iterators

private:
	using mono_list = mono::list<T, N>;
	using list_type = intrusive_list<mono_list>;
	using node_type = typename list_type::node;
	list_type list_;
};
} // namspace duo
} // namespace haisu


