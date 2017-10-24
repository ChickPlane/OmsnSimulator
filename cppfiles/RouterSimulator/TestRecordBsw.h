#pragma once
#include "TestRecord.h"
#include "SimulatorCommon.h"

enum {
	REC_ST_GENERATE = 0,
	REC_ST_FIRSTSEND,
	REC_ST_REACH,
	REC_ST_REP_LEAVE,
	REC_ST_REP_RETURN,
	REC_ST_MAX
};

class CTestRecordBsw :
	public CTestRecord
{
public:
	CTestRecordBsw();
	virtual ~CTestRecordBsw();

	SIM_TIME m_lnTimes[REC_ST_MAX];
};