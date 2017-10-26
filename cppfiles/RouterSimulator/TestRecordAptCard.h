#pragma once
#include "SimulatorCommon.h"
#include "TestRecord.h"

enum {
	REC_AC_ST_GENERATE = 0,
	REC_AC_ST_FIRSTSEND,
	REC_AC_ST_REACH,
	REC_AC_ST_REP_LEAVE,
	REC_AC_ST_REP_RETURN,
	REC_AC_ST_MAX
};

class CTestRecordAptCard :
	public CTestRecord
{
public:
	CTestRecordAptCard()
	{
		for (int i = 0; i < REC_AC_ST_MAX; ++i)
		{
			m_lnTimes[i] = -1;
		}
	}
	SIM_TIME m_lnTimes[REC_AC_ST_MAX];
};

