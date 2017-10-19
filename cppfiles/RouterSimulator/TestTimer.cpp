#include "stdafx.h"
#include "TestTimer.h"


CTestTimer::CTestTimer()
{
}


CTestTimer::~CTestTimer()
{
}

void CTestTimer::OnCommonTimer(UINT_PTR nIDEvent)
{
	int a = 0;
}

void CTestTimer::StartTest()
{
	SetCommonTimer(1, 500, TRUE);
}
