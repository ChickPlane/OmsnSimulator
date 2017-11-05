#pragma once
#include "SimulatorCommon.h"

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
	int m_nForwardNumber;
	BOOL m_bInStatistic;
};

