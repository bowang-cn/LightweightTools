#pragma once
#include <iostream>
#include <atomic>
#include "../LightweightTools/LongStandingThread.h"
#include "MyObj.h"

static void ThreadFun(const std::atomic<int> &a)
{
	std::cout << "Fun " << a<< std::endl;
}

static void TestGlobleFun()
{
	CLongStandingThread aJoinThread;

	std::atomic<int> nIndex = 0;
	aJoinThread.StartTimed(std::chrono::seconds(1), ThreadFun, std::ref(nIndex));

	for (size_t i = 0; i < 5; i++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		++nIndex;
	}

	aJoinThread.RequestStop();
}

static void TestMemberFun()
{
	CLongStandingThread aJoinThread;
	CMyObj aObj;
	int nIndex = 1024;
	aJoinThread.StartTimed(std::chrono::seconds(1), &CMyObj::Print, &aObj, nIndex);

	for (size_t i = 0; i < 5; i++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	aJoinThread.RequestStop();
}

static void TestWakeable()
{
	CLongStandingThread aJoinThread;
	std::atomic<int> nIndex = 0;
	aJoinThread.StartWakeable(ThreadFun, std::ref(nIndex));
	for (size_t i = 0; i < 5; i++)
	{
		aJoinThread.Wake();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		++nIndex;
	}
	aJoinThread.RequestStop();
}

static void TestTimed()
{
	//TestGlobleFun();
	TestMemberFun();
}

static void TestFun(bool bTimeout, int a)
{
	std::cout << "TestFun " << a << " bTimeout:" << bTimeout << std::endl;
}

static void TestWakeableWithTimed()
{
	CLongStandingThread aJoinThread;
	int nIndex = 0;
	aJoinThread.StartWakeableWithTimed(std::chrono::seconds(3), TestFun, std::placeholders::_1, nIndex);
	for (size_t i = 0; i < 5; i++)
	{
		aJoinThread.Wake();
		std::this_thread::sleep_for(std::chrono::seconds(i + 1));
	}
	aJoinThread.RequestStop();
}

static void TestJoinThread()
{
	//TestWakeable();
	//TestTimed();
	TestWakeableWithTimed();
}