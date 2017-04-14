#pragma once

#include <limits>
#include <vector>
#include <cstring>

namespace haisu
{
namespace algo
{

static const size_t npos = std::numeric_limits<size_t>::max();

template <typename T>
struct three_way_cmp
{
	int operator ()(T lhs, T rhs) const
	{
		if (lhs < rhs)
		{
			return -1;
		}
		else if (lhs > rhs)
		{
			return 1;
		}

		return 0;
	}
};

template <>
struct three_way_cmp<const char*>
{
	int operator ()(const char* lhs, const char* rhs) const
	{
		return strcmp(lhs, rhs);
	}
};

template <>
struct three_way_cmp<std::string>
{
	int operator ()(const std::string& lhs, const std::string& rhs) const
	{
		return lhs.compare(rhs);
	}
};

template <typename Container, typename Key, typename Cmp = three_way_cmp<Key>>
size_t binary_search(const Container& container, Key key, Cmp cmp = Cmp())
{
	if (container.empty()) return npos;

	int head = 0;
	int tail = container.size() - 1;

	while (head <= tail)
	{
		int mid = (head + tail) / 2;
		const int res = cmp(key, container.at(mid));
		if (res > 0)
		{
			head = mid + 1;
		}
		else if (res < 0)
		{
			tail = mid - 1;
		}
		else
		{
			return mid;
		}
	}
		
	return npos;
}

}
}
