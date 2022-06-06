#pragma once
#include <cstdlib>
#include <ctime>

// SkipList's level creator,
struct CSkipListLevelCreator
{
	CSkipListLevelCreator() { std::srand(unsigned(std::time(0))); }

	int operator() (int nMaxLevel) const
	{
		int nLevel = 0;
		// The probability of rising from k to k+1 level is 0.25
		while ((std::rand() & 127) < 32 && nLevel < nMaxLevel)
		{
			++nLevel;
		}

		return nLevel;
	}
};