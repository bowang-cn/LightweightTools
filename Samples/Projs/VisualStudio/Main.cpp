// Main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "../../TestJoinThread.h"
#include "../../TestSingleton.h"
#include "../../TestObjectPool.h"
#include "../../TestSkipList.h"

int main()
{
	//TestObjectPool();
	//TestSingleton();
	//TestJoinThread();
	TestSkipList();
	system("pause");
    return 0;
}

