#pragma once
#include "CommonTimer.h"
class CTestTimer
	: CCommonTimer
{
public:
	CTestTimer();
	virtual ~CTestTimer();
	virtual void OnCommonTimer(UINT_PTR nIDEvent);
	void StartTest();
};

