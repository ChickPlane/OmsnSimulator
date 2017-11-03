#pragma once
#include "TestRecord.h"
#include "SimulatorCommon.h"

enum {
	REC_MHLPP_ST_GENERATE,
	REC_MHLPP_ST_FIRSTSEND,
	REC_MHLPP_ST_OBFUSCATION_OVER,
	REC_MHLPP_ST_REACH,
	REC_MHLPP_ST_REP_LEAVE,
	REC_MHLPP_ST_REP_RETURN,
	REC_MHLPP_ST_MAX
};

enum {
	SUM_MHLPP_ST_FORWARD,
	SUM_MHLPP_MAX
};

class CTestRecordMhlpp :
	public CTestRecord
{
public:
	CTestRecordMhlpp();
	virtual ~CTestRecordMhlpp();
	SIM_TIME m_lnTimes[REC_MHLPP_ST_MAX];
};

