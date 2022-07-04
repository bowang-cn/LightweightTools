#pragma once
#include <iostream>
#include "../LightweightTools/ThreadPool.h"

static void PrintInt(int a)
{
	std::cout << a << std::endl;
}

static int Sum(int a, int b)
{
	return a + b;
}

static void TestThreadPool()
{
	CThreadPool aThreadPool(2);

	aThreadPool.Initialize();
	aThreadPool.Submit(PrintInt, 2);
	auto fSum = aThreadPool.Submit(Sum, 10, 5);

	std::cout << fSum.get() << std::endl;
	std::cout << "task count : " << aThreadPool.TotalTaskCount() << std::endl;

	aThreadPool.Uninitialize();
}