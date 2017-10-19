#pragma once
#include "SimulatorCommon.h"
class CQueryMission
{
public:
	CQueryMission();
	CQueryMission(const CQueryMission & src);
	CQueryMission & operator=(const CQueryMission & src);
	virtual ~CQueryMission();
	virtual void ChangeID();

	int m_nMissionId;
	USERID m_SenderId;
	USERID m_RecverId;
	SIM_TIME m_lnTimeOut;

private:
	static int sm_nMissionIdMax;
};

