#pragma once

#include "NonCopyable.h"

template <class T>
class CSingleton : private CNonCopyable
{
	class CObjectCreator
	{
	public:
		CObjectCreator()
		{
			CSingleton<T>::GetInstance();
		}

	public:
		inline void DoNothing() const {}
	};

protected:
	CSingleton() = default;
	virtual ~CSingleton() = default;

public:
	static T& GetInstance()
	{
		static T aInstance;

		m_ObjectCreator.DoNothing();

		return aInstance;
	}

private:
	static CObjectCreator m_ObjectCreator;
};

template <class T>
typename CSingleton<T>::CObjectCreator CSingleton<T>::m_ObjectCreator;