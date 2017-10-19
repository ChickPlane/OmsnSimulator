#pragma once
#include "TestRecord.h"
#include "SimulatorCommon.h"

enum {
	REC_APT_ST_GENERATE = 0,
	REC_APT_ST_FIRSTSEND,
	REC_APT_ST_REACH,
	REC_APT_ST_REP_LEAVE,
	REC_APT_ST_REP_RETURN,
	REC_APT_ST_MAX
};

class CTestRecordAptCard :
	public CTestRecord
{
public:
	CTestRecordAptCard();
	virtual ~CTestRecordAptCard();

	SIM_TIME m_lnTimes[REC_APT_ST_MAX];
};

