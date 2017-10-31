#pragma once
#include "TestRecord.h"
#include "SimulatorCommon.h"

enum {
	REC_SLPD_ST_GENERATE,
	REC_SLPD_ST_FIRSTSEND,
	REC_SLPD_ST_OBFUSCATION_OVER,
	REC_SLPD_ST_REACH,
	REC_SLPD_ST_REP_LEAVE,
	REC_SLPD_ST_REP_RETURN,
	REC_SLPD_ST_MAX
};

enum {
	SUM_SLPD_ST_FORWARD,
	SUM_SLPD_MAX
};

class CTestRecordSlpd :
	public CTestRecord
{
public:
	CTestRecordSlpd();
	virtual ~CTestRecordSlpd();

	SIM_TIME m_lnTimes[REC_SLPD_ST_MAX];
};

