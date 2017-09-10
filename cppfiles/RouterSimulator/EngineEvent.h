#pragma once
#include "EngineUser.h"
#include "SimulatorCommon.h"
class CEngineEvent
{
public:
	CEngineEvent();
	CEngineEvent(const CEngineEvent & src);
	CEngineEvent & operator = (const CEngineEvent & src);
	~CEngineEvent();

	CEngineUser * m_pUser;
	SIM_TIME m_lnSimTime;
	int m_nCommandId;
};

