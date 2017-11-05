#pragma once
#include "SimulatorCommon.h"

enum
{
	ENGINE_RECORD_FORWARD_TIMES = 0,
	ENGINE_RECORD_MAX
};
class CTestRecord
{
public:
	CTestRecord();
	CTestRecord(int nMilestoneNumber);
	CTestRecord(const CTestRecord & src);
	CTestRecord & operator=(const CTestRecord & src);
	virtual ~CTestRecord();

	void Reset();

	int m_nSessionId;
	int m_nForwardTimes;
	SIM_TIME * m_pMilestoneTime;
	int m_nMilestoneNumber;
};

