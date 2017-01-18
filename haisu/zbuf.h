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
		
		memmove(&buf[(index + 1)* sz], &buf[index * sz], prev - index * sz);
		at<T>(index) = t;
	}

	template <typename T>
	void erase(size_t index)
	{
		memmove(&buf[index* sizeof(T)], &buf[(index + 1)* sizeof(T)], buf.size() - sizeof(T));
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
	memmove(&buf[index], &buf[from], buf.size() - from);
	buf.resize(buf.size() - sz);
}

template <> inline
void zbuf::insert<const char*>(size_t index, const char* t)
{
	const size_t prev = buf.size();
	const size_t sz = strlen(t) + 1;
	buf.resize(prev + sz);
		
	memmove(&buf[index + sz], &buf[index], prev - index);
	memcpy(&buf[index], t, sz);
}


}

