#include "stdafx.h"
#include "MyObj.h"
#include <iostream>

CMyObj::CMyObj()
{
}


CMyObj::~CMyObj()
{
}

void CMyObj::Print(int a)
{
	std::cout << "CMyObj::Print " << a << std::endl;
}
