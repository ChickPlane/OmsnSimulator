#pragma once
#include "SimulatorCommon.h"
#include "TestRecord.h"

enum {
	REC_AC_ST_GENERATE,
	REC_AC_ST_FIRSTSEND,
	REC_AC_ST_REACH,
	REC_AC_ST_REP_LEAVE,
	REC_AC_ST_REP_RETURN,
	REC_AC_ST_MAX
};

enum 
{
	REC_AC_PTL_EXIST_CREATED_CARD_NUMBER,
	REC_AC_PTL_HOLDING_TRUST_CARD_NUMBER,
	REC_AC_PTL_SENT_CARDPKG_NUMBER,
	REC_AC_PTL_SENT_CARD_NUMBER,
	REC_AC_PTL_MAX
};