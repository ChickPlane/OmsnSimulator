#pragma once
#include "SimulatorCommon.h"
#include "TestSessionBsw.h"

class CTestSessionAptCard :
	public CTestSessionBsw
{
public:
	CTestSessionAptCard() {}
	CTestSessionAptCard(const CTestSessionAptCard & src) : CTestSessionBsw(src) {}
};