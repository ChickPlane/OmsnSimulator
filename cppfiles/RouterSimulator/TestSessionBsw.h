#pragma once
#include "TestSession.h"
#include "SimulatorCommon.h"
class CTestSessionBsw :
	public CTestSession
{
public:
	CTestSessionBsw() {}
	CTestSessionBsw(const CTestSessionBsw & src) : CTestSession(src) {}
};

