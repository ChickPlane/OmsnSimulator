#pragma once
#include "SimulatorCommon.h"

class CTestRecord;

class CTestSession
{
public:
	CTestSession();
	CTestSession(const CTestSession & src);
	virtual CTestSession & operator = (const CTestSession & src);
	virtual ~CTestSession();

	void InitSession(int nSessionId);

	int m_nSessionId;
	SIM_TIME m_lnTimeOut;
};

