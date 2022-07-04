#pragma once
#include <map>
#include <functional>

template<class k, class v> using defalut_function_factory_container = std::map<k, v>;
template<class Key, class Op, template<class k, class v> class Container = defalut_function_factory_container>
class CFunctionFactory
{
	typedef std::function<Op> function_type;
	typedef Container<Key, function_type> container_type;
	
public:
	CFunctionFactory() : m_aDefault(nullptr) {}
	~CFunctionFactory() {}

	bool AddFunction(Key aKey, function_type aFunction)
	{
		bool bResult = false;
	
		try
		{
			m_mapFactory.emplace(std::move(aKey), std::move(aFunction));
			bResult = true;
		}
		catch (...)
		{
			;
		}

		return bResult;
	}

	void SetDefault(function_type aDefault)
	{
		m_aDefault = std::move(aDefault);
	}

	function_type operator[](const Key &aKey) const
	{
		return Get(aKey, m_aDefault);
	}

	function_type Get(const Key &aKey, function_type aDefault) const
	{
		auto iter = m_mapFactory.find(aKey);
		return (m_mapFactory.end() != iter) ? iter->second : aDefault;
	}

	void Clear()
	{
		m_mapFactory.clear();
		m_aDefault = nullptr;
	}

private:
	function_type m_aDefault;
	container_type m_mapFactory;
};