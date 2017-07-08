#pragma once

#include <algorithm>
#include "algo.h"
#include "zbuf.h"

namespace haisu
{

class zset
{
public:
	typedef const char* key_type;
	typedef const char* value_type;
	typedef std::size_t size_type;
	typedef int off_t;

	class iterator
	{
	public:
		typedef zset::value_type value_type;

		iterator() : z(nullptr), pos(-1) {}

		zset::value_type operator *() const
		{
			return z->at(pos);
		}

		bool operator ==(const iterator& other) const noexcept
		{
			return (end() && other.end()) || (other.z == z && other.pos == pos);
		}

		bool operator != (const iterator& other) const noexcept
		{
			return !(*this == other);
		}

		iterator& operator ++()
		{
			++pos;

			pos = pos < z->_size ? pos : -1;

			return *this;
		}

		iterator operator ++(int)
		{
			iterator prev(*this);
			++(*this);
			return prev;
		}

	private:
		size_t index() const
		{
			return static_cast<size_t>(pos);
		}

		bool end() const
		{
			return z == NULL || pos >= z->_size;
		}

		iterator(const zset& z, int p) : z(&z), pos(p) { }
		const zset* z;
		int pos;

		friend zset;
	};

	typedef iterator const_iterator;
	

	zset()
		: _size(0)
	{
	}

	zset(std::initializer_list<key_type> ll)
	{
		for (auto l : ll)
		{
			insert(l);
		}
	}

	bool empty() const noexcept
	{
		return 0 == _size;
	}

	size_t size() const noexcept
	{
		return _size;
	}

	size_t max_size() const noexcept
	{
		return _size;
	}

	size_t capacity() const noexcept
	{
		return _buf.capacity();
	}

	void reserve(size_t bytes)
	{
		_buf.reserve(bytes);
	}

	void insert(value_type value)
	{
		iterator i = lower_bound(value);

		if (i == end())
		{
			for (size_t i = 0; i < _size; ++i)
			{
				_buf.at<off_t>(i) += sizeof(off_t);
			}

			_buf.insert(_size, static_cast<off_t>(_buf.size() + sizeof(off_t)));
			_buf.append(value);

			++_size;
		}
		else if (0 != strcmp(*i, value))
		{
			const size_t len = strlen(value) + 1;
			_buf.grow(len + sizeof(off_t));

			const off_t idx = i.index();
			const off_t off = _buf.at<off_t>(idx);
			
			_buf.insert(off, value);
			
			for (size_t i = 0; i < idx; ++i)
			{
				_buf.at<off_t>(i) += sizeof(off_t);
			}

			for (size_t i = idx; i < _size; ++i)
			{
				_buf.at<off_t>(i) += len + sizeof(off_t);
			}

			_buf.insert(idx, off + static_cast<off_t>(sizeof(off_t)));
			++_size;
		}
	}

	value_type at(size_t index) const
	{
		return _buf.ptr(_buf.at<off_t>(index));
	}

	void clear() noexcept
	{
		_buf.clear();
		_size = 0;
	}

	void erase(value_type value)
	{
		size_t found = find_pos(value);
		if (found != npos())
		{
			_buf.erase<const char*>(_buf.at<off_t>(found));
			if (found + 1 < _size)
			{
				const size_t diff = _buf.at<off_t>(found + 1) - _buf.at<off_t>(found) + sizeof(off_t);

				for (int i = found + 1; i < _size; ++i)
				{
					_buf.at<off_t>(i) -= diff;
				}
			}

			for (int i = 0; i < found; ++i)
			{
				_buf.at<off_t>(i) -= sizeof(off_t);
			}

			_buf.erase<off_t>(found);
			--_size;
		}
	}

	size_t count(value_type value) const
	{
		return find_pos(value) == npos() ? 0 : 1;
	}

	const_iterator lower_bound(key_type key) const noexcept
	{
		return std::lower_bound(begin(), end(), key, 
			[](auto lhs, auto rhs){return strcmp(lhs, rhs) < 0;});
	}

	const_iterator upper_bound(key_type key) const noexcept
	{
		return std::upper_bound(begin(), end(), key, 
			[](auto lhs, auto rhs){return strcmp(lhs, rhs) < 0;});
	}

	const_iterator end() const noexcept
	{
		return iterator(*this, -1);
	}

	const_iterator begin() const noexcept
	{
		if (_size > 0)
		{
			return iterator(*this, 0);
		}
		
		return end();
	}

	const_iterator cend() const noexcept
	{
		return end();
	}

	const_iterator cbegin() const noexcept
	{
		return begin();
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type key) const noexcept
	{
		return std::equal_range(begin(), end(), key, 
			[](auto lhs, auto rhs){return strcmp(lhs, rhs) < 0;});
	}

	const_iterator find(key_type key) const noexcept
	{
		size_t found = find_pos(key);
		if (found != npos())
		{
			return iterator(*this, found);
		}

		return end();
	}

private:

	size_t find_pos(key_type key) const
	{
		return algo::binary_search(*this, key);
	}

	size_t size_at(size_t pos) const
	{
		if (pos != _size - 1)
		{
			return _buf.at<off_t>(pos + 1) - _buf.at<off_t>(pos);
		}
		else
		{
			return _buf.size() - _buf.at<off_t>(pos) - 1;
		}
	}

	static size_t npos()
	{
		return std::numeric_limits<size_t>::max();
	}

	zbuf _buf;
	size_t _size = 0;
};

}

namespace std
{
template <> struct iterator_traits<haisu::zset::iterator>
{
	typedef haisu::zset::value_type value_type;
	typedef int difference_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef std::forward_iterator_tag iterator_category;
};
}


