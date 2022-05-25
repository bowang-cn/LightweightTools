#pragma once
#include <iostream>
#include "../LightweightTools/Singleton.h"

class CUniqueObj : public CSingleton<CUniqueObj>
{
public:
	CUniqueObj() {}
	~CUniqueObj() {}

	void Print() const
	{
		std::cout << "CUniqueObj::Print" << std::endl;
	}
};

static void TestSingleton()
{
	CUniqueObj::GetInstance().Print();
}