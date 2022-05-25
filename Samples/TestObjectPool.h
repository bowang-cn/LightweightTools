#pragma once
#include <assert.h>
#include <iostream>
#include "../LightweightTools/ObjectPool.h"
#include "../LightweightTools/Singleton.h"

class CBaseObj
{
public:
	CBaseObj() { std::cout << "CBaseObj() " << std::endl; }
	virtual ~CBaseObj() 
	{
		std::cout << "~CBaseObj() " << std::endl;
	}
};

class CDerived : public CBaseObj
{
public:
	CDerived(int nData) :CBaseObj(), m_nData(nData)
	{
		std::cout << "CDerived()" << std::endl;
	}
	~CDerived()
	{
		std::cout << "~CDerived()" << std::endl;
	}

	void PrintData() const
	{
		std::cout << "Data :" << m_nData << std::endl;
	}

private:
	int m_nData;
};

using CDerivedFactory = CSingleton<CObjectPool<CDerived>>;

static void TestObjectPool()
{
	CDerived *pObj = CDerivedFactory::GetInstance().Create(1024);
	assert(nullptr != pObj);
	pObj->PrintData();

	CBaseObj *pBase = static_cast<CBaseObj*>(pObj);
	CDerivedFactory::GetInstance().Release(pBase);
}