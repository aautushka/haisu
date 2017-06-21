#pragma once

#include <vector>
#include <cstring>

namespace haisu
{

class zbuf
{
public:
	template <typename T> 
	void append(T t)
	{
		const size_t prev = buf.size();
		const size_t sz = sizeof(t);
		buf.resize(prev + sz);
		memcpy(&buf[prev], &t, sz);
	}

	template <typename T>
	T at(size_t index) const
	{
		return *reinterpret_cast<const T*>(&buf[index * sizeof(T)]);
	}

	template <typename T>
	T& at(size_t index)
	{
		return *reinterpret_cast<T*>(&buf[index * sizeof(T)]);
	}

	template <typename T>
	void insert(size_t index, T t)
	{
		const size_t prev = buf.size();
		const size_t sz = sizeof(t);
		buf.resize(prev + sz);
		
        auto source = &buf[index * sz];
        auto dest = source + sz;
		memmove(dest, source, prev - index * sz);
		at<T>(index) = t;
	}

	template <typename T>
	void erase(size_t index)
	{
		const auto dest = index * sizeof(T);
		const auto src = dest + sizeof(T);
		const auto size = buf.size() - src;
        
        auto destptr = &buf[dest];
        auto sourceptr = destptr + sizeof(T);

        memmove(destptr, sourceptr, size);
		buf.resize(buf.size() - sizeof(T));
	}

	size_t size() const noexcept
	{
		return buf.size();
	}

	size_t capacity() const noexcept
	{
		return buf.capacity();
	}

	const char* ptr(size_t index) const
	{
		return &buf[index];
	}

	void reserve(size_t bytes)
	{
		buf.reserve(bytes);
	}

	void grow(size_t delta)
	{
		reserve(buf.size() + delta);
	}

	void clear() noexcept
	{
		buf.clear();
	}

private:
	std::vector<char> buf;
};

template <> inline
void zbuf::append<const char*>(const char* t)
{
	const size_t prev = buf.size();
	const size_t sz = strlen(t) + 1;
	buf.resize(prev + sz);
	memcpy(&buf[prev], t, sz);
}

template <> inline
void zbuf::erase<const char*>(size_t index)
{
	const size_t sz = strlen(&buf[index]) + 1;
	const size_t from = index + sz;
    
    auto dest = &buf[index];
    auto source = dest + sz;
	memmove(dest, source, buf.size() - from);
	buf.resize(buf.size() - sz);
}

template <> inline
void zbuf::insert<const char*>(size_t index, const char* t)
{
	const size_t prev = buf.size();
	const size_t sz = strlen(t) + 1;
	buf.resize(prev + sz);
		
    auto source = &buf[index];
    auto dest = source + sz;
    memmove(dest, source, prev - index);
    memcpy(source, t, sz);
}


}

