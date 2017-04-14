#pragma once
#include <memory>
#include <map>

namespace haisu
{
template <typename Key, typename Val>
class tree
{
	using leaves_t = std::map<Key, std::unique_ptr<tree>>;	

public:
	using self_type = tree<Key, Val>;
	using value_type = Val;
	using key_type = Key;

	using iterator = typename leaves_t::iterator;
	using const_iterator = typename leaves_t::const_iterator;

	tree()
	{
	}

	tree(const tree& other) = delete;
	tree& operator =(const tree& other) = delete;

	template <typename T>
	value_type& operator [](const T& key)
	{
		return child(key)->get();
	}

	template <typename T>
	const value_type& operator [](const std::vector<key_type>& key) const
	{
		return child(key)->get();
	}

	value_type& operator [](const key_type& key)
	{
		return child(key)->get();
	}

	const value_type& operator [](const key_type& key) const
	{
		return child(key)->get();
	}

	void insert(const key_type& key, const value_type& val)
	{
		child(key) = val;
	}

	bool empty() const
	{
		return _leaves.empty();
	}

	iterator begin() 
	{
		return iterator(_leaves.begin());
	}

	iterator end()
	{
		return iterator(_leaves.end());
	}

	const_iterator begin() const
	{
		return _leaves.begin();
	}

	const_iterator end() const
	{
		return _leaves.end();
	}

	template <typename Func>
	void foreach(Func func) const
	{
		for (auto i = begin(); i != end(); ++i)
		{
			func(i->first, i->second->get());	
			i->second->foreach(func);
		}
	}

	value_type& get()
	{
		return _val;
	}

	const value_type& get() const
	{
		return _val;
	}

private:
	// TODO: gls::not_null
	tree* child(const key_type& key)
	{
		auto i = _leaves.find(key);
		if (i != _leaves.end())
		{
			return i->second.get();
		}
		else
		{
			std::unique_ptr<self_type> res(new tree);
			auto ptr = res.get();
			_leaves[key] = std::move(res);
			return ptr;
		}
	}

	// TODO: gsl::not_null
	const tree* child(const key_type& key) const
	{
		auto i = _leaves.find(key);
		if (i != _leaves.end())
		{
			return i->second.get();
		}

		throw std::exception();
	}

	// TODO: gsl::not_null
	template <typename T>
	tree* child(const T& key)
	{
		self_type* tr = this;
		for (auto k : key) tr = tr->child(k);
		return tr;
	}

	// TODO: gls::not_null
	template <typename T>
	const tree* child(const T& key) const
	{
		self_type* tr = this;
		for (auto k : key) tr = tr->child(k);
		return tr;
	}

	value_type _val;
	leaves_t _leaves;	
}; 
} // namespace haisu

