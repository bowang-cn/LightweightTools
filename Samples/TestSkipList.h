#pragma once
#include <ctime>
#include <iostream>
#include <vector>
#include <assert.h>
#include "../LightweightTools/SkipList/SkipList.h"

template<class Key, class Value>
using SkipList_t = CSkipList<Key, Value, 5>;
using SkipList_int = SkipList_t<int, int>;

template<class F, class S>
void Print(const std::pair<F, S> &pair)
{
	std::cout << "[" << pair.first << "," << pair.second << "]" << std::endl;
}

template<unsigned int nRange = 50>
int RandomKey() { return std::rand() % nRange; }

void TestSkipListInsert()
{
	std::srand(unsigned(std::time(0)));

	SkipList_int aSkipList;
	for (int i = 0; i < 20; i++)
	{
		int nKey = RandomKey();
		if (!aSkipList.insert(nKey, i))
		{
			 std::cout << nKey << "  " << i << std::endl;
		}
	}
}

void TestSkipListIterator()
{
	std::srand(unsigned(std::time(0)));

	SkipList_int aSkipList;
	for (int i = 0; i < 5; i++)
	{
		int nKey = RandomKey();
		if (!aSkipList.insert(nKey, i))
		{
			std::cout << nKey << "  " << i << std::endl;
		}
	}

	std::cout << "test iterator->,value to 1024" << std::endl;
	for (auto iter = aSkipList.begin(); iter != aSkipList.end(); ++iter)
	{
		std::cout << "[" << iter->first << "," << iter->second << "]" << std::endl;
		// iter->first = 23;
		iter->second = 1024;
	}

	std::cout << "test iterator*,value to 33333" << std::endl;
	for (auto iter = aSkipList.begin(); iter != aSkipList.end(); ++iter)
	{
		std::cout << "[" << (*iter).first << "," << (*iter).second << "]" << std::endl;
		// (*iter).first = 23;
		(*iter).second = 33333;
	}

	std::cout << std::endl;
}

void TestSkipListConstIterator()
{
	std::srand(unsigned(std::time(0)));

	SkipList_int aSkipList;
	for (int i = 0; i < 5; i++)
	{
		int nKey = RandomKey();
		if (!aSkipList.insert(nKey, i))
		{
			std::cout << nKey << "  " << i << std::endl;
		}
	}
	std::cout << std::endl;

	const SkipList_int &aSkipList_c = aSkipList;
	std::cout << "iterator " << (aSkipList.end() == aSkipList_c.end() ? "==" : "!=") << " const_iterator" << std::endl;
	std::cout << std::endl;

	std::cout << "test const_iterator->" << std::endl;
	for (auto iter = aSkipList_c.begin(); aSkipList_c.end() != iter; ++iter)
	{
		std::cout << "[" << iter->first << "," << iter->second << "]" << std::endl;
// 		iter->first = 23;
// 		iter->second = 1024;
	}
	std::cout << std::endl;

	std::cout << "test const_iterator*" << std::endl;
	for (auto iter = aSkipList_c.begin(); iter != aSkipList_c.end(); ++iter)
	{
		std::cout << "[" << (*iter).first << "," << (*iter).second << "]" << std::endl;
//		(*iter).first = 23;
//		(*iter).second = 33333;
	}
}

void TestSkipListFind()
{
	std::srand(unsigned(std::time(0)));

	SkipList_int aSkipList;
	for (int i = 0; i < 20; i++)
	{
		int nKey = i;
		if (!aSkipList.insert(nKey, i))
		{
			std::cout << nKey << "  " << i << std::endl;
		}
	}
	assert(20 == aSkipList.size());
	aSkipList.insert(101, 102);
	assert(21 == aSkipList.size());
	std::cout << std::endl;

	auto rFind = aSkipList.find(101);
	assert(rFind.begin() != rFind.end() && 101 == rFind.begin()->first && 102 == rFind.begin()->second);
//	rFind.Begin()->first = 123;
	rFind.begin()->second = 1024;

	auto rFind100 = aSkipList.find(100);
	assert(rFind100.begin() == rFind100.end());

	int nFrom = 15, nTo = 300;
	auto rFindMulti = aSkipList.find(nFrom, nTo);
	std::cout << "Find[" << nFrom << "," << nTo << "):" << std::endl;
	for (auto iter = rFindMulti.begin(), end = rFindMulti.end(); end != iter; ++iter)
	{
		Print(*iter);
	}

	nFrom = 10, nTo = 15;
	rFindMulti = aSkipList.find(nFrom, nTo);
	std::cout << "Find[" << nFrom << "," << nTo << "):" << std::endl;
	for (auto iter = rFindMulti.begin(), end = rFindMulti.end(); end != iter; ++iter)
	{
		Print(*iter);
	}

	nFrom = -5, nTo = 5;
	rFindMulti = aSkipList.find(nFrom, nTo);
	std::cout << "Find[" << nFrom << "," << nTo << "):" << std::endl;
	for (auto iter = rFindMulti.begin(), end = rFindMulti.end(); end != iter; ++iter)
	{
		Print(*iter);
	}
}

void TestSkipListFind_const()
{
	std::srand(unsigned(std::time(0)));

	SkipList_int aSkipList;
	for (int i = 0; i < 20; i++)
	{
		int nKey = i;
		if (!aSkipList.insert(nKey, i))
		{
			std::cout << nKey << "  " << i << std::endl;
		}
	}
	assert(20 == aSkipList.size());
	aSkipList.insert(101, 102);
	assert(21 == aSkipList.size());
	std::cout << std::endl;

	const SkipList_int &aSkipList_c = aSkipList;
	auto rFind = aSkipList_c.find(101);
	assert(rFind.begin() != rFind.end() && 101 == rFind.begin()->first && 102 == rFind.begin()->second);
//	rFind.Begin()->second = 1024;

	auto rFind100 = aSkipList_c.find(100);
	assert(rFind100.begin() == rFind100.end());

	int nFrom = 15, nTo = 300;
	auto rFindMulti = aSkipList_c.find(nFrom, nTo);
	std::cout << "Find[" << nFrom << "," << nTo << "):" << std::endl;
	for (auto iter = rFindMulti.begin(), end = rFindMulti.end(); end != iter; ++iter)
	{
		Print(*iter);
	}

	nFrom = 10, nTo = 15;
	rFindMulti = aSkipList_c.find(nFrom, nTo);
	std::cout << "Find[" << nFrom << "," << nTo << "):" << std::endl;
	for (auto iter = rFindMulti.begin(), end = rFindMulti.end(); end != iter; ++iter)
	{
		Print(*iter);
	}

	nFrom = -5, nTo = 5;
	rFindMulti = aSkipList_c.find(nFrom, nTo);
	std::cout << "Find[" << nFrom << "," << nTo << "):" << std::endl;
	for (auto iter = rFindMulti.begin(), end = rFindMulti.end(); end != iter; ++iter)
	{
		Print(*iter);
	}
}

#include <string>
struct SKey 
{
	int m_nID;
	std::string m_sData;
	SKey(int nId, const std::string &sData) :m_nID(nId), m_sData(sData) {}
};
bool operator== (const SKey &k1, const SKey &k2)
{
	return k1.m_nID == k2.m_nID;
}
bool operator< (const SKey &k1, const SKey &k2)
{
	return k1.m_nID > k2.m_nID;
}

struct SData
{
	std::string m_sValue;
	SData(const std::string &sValue) :m_sValue(sValue) {}
};

using SkipList_user = CSkipList<SKey, SData>;
void TestUserData()
{
	SkipList_user aSkipList;
	for (int i = 0; i < 5; i++)
	{
		aSkipList.insert(SKey(111 * i, "第" + std::to_string(i) + "个Key"), SData("第" + std::to_string(i) + "个Value"));
	}
	
	for (const auto &v : aSkipList)
	{
		std::cout << "[" << v.first.m_nID << ":" << v.first.m_sData.c_str() <<  "," << v.second.m_sValue.c_str() << "]" << std::endl;
	}
}

void TestSkipListErase()
{
	SkipList_int aSkipList_s;
	assert(0 == aSkipList_s.size());
	for (int i = 0; i < 1; i++)
	{
		int nKey = i;
		aSkipList_s.insert(nKey, i);
	}
	assert(1 == aSkipList_s.size());
	assert(1 == aSkipList_s.erase(0));
	assert(0 == aSkipList_s.size());
	for (auto iter = aSkipList_s.begin(); iter != aSkipList_s.end(); ++iter)
	{
		assert(false);
	}

	SkipList_int aSkipList;
	for (int i = 0; i < 10; i++)
	{
		int nKey = i;
		if (!aSkipList.insert(nKey, i))
		{
			std::cout << nKey << "  " << i << std::endl;
		}
	}
	std::cout << std::endl;

	aSkipList.erase(9);

	std::cout << std::endl;
	aSkipList.erase(0);

	std::cout << std::endl;
	aSkipList.erase(5);

	std::cout << std::endl;
	aSkipList.erase(7);

	std::cout << std::endl;
	aSkipList.erase(6);

	std::cout << std::endl;
	aSkipList.erase(4);

	std::cout << std::endl;
	aSkipList.erase(2);

	std::cout << std::endl;
	aSkipList.erase(8);
	
	assert(2 == aSkipList.size());
	auto iter = aSkipList.begin();
	assert(1 == iter->first);
	++iter;
	assert(3 == iter->first);
}

void TestSkipListOther()
{
	SkipList_int aSkipList;
	for (int i = 0; i < 10; i++)
	{
		int nKey = i;
		aSkipList.insert(nKey, i);
	}

	std::cout << "single find 4" << std::endl;
	auto rFind4 = aSkipList.find(4);
	for (auto &&v : rFind4)
	{
		Print(v);
	}
	std::cout << std::endl;

	// range-for
	std::cout << "range-for" << std::endl;
	for (auto &&v : aSkipList)
	{
		Print(v);
	}
	std::cout << std::endl;

	// Range range-for
	std::cout << "Range range-for" << std::endl;
	auto rResult = aSkipList.find(0, 5);
	for (auto &&v : rResult)
	{
		Print(v);
	}
	std::cout << std::endl;

	std::cout << "const range-for" << std::endl;
	const SkipList_int &aSkipList_c = aSkipList;
	for (auto &&v : aSkipList_c)
	{
		Print(v);
	}
	std::cout << std::endl;

	std::cout << "const single find 4" << std::endl;
	auto rFind4c = aSkipList_c.find(4);
	for (auto &&v : rFind4c)
	{
		Print(v);
	}
	std::cout << std::endl;

	// Range range-for
	std::cout << "const Range range-for" << std::endl;
	auto rResult_c = aSkipList_c.find(0, 5);
	for (auto &&v : rResult_c)
	{
		Print(v);
	}
	std::cout << std::endl;

	// move construct
	std::cout << "move construct  ***************************" << std::endl;
	SkipList_int aSkipList2(std::move(aSkipList));
	std::cout << "aSkipList" << std::endl;
	for (auto &&v : aSkipList)
	{
		Print(v);
	}
	std::cout << std::endl;

	std::cout << "aSkipList2" << std::endl;
	for (auto &&v : aSkipList2)
	{
		Print(v);
	}
	std::cout << std::endl;

	// aSkipList = aSkipList2;   // 禁用
}

// SkipList测试函数
void TestSkipList()
{
 	TestSkipListInsert();
	TestSkipListIterator();
	TestSkipListConstIterator();
	TestSkipListFind();
	TestSkipListFind_const();
	TestUserData();
	TestSkipListErase();
	TestSkipListOther();
}