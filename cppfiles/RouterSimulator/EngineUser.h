#pragma once
#include "SimulatorCommon.h"
#include "StatisticSummary.h"

class CHostEngine;

class CEngineUser
{
public:
	CEngineUser();
	virtual ~CEngineUser();

	virtual void OnEngineTimer(int nCommandId) = 0;
	virtual void OnEngineSpeedChanged();
	virtual void OnEngineTimeChanged(SIM_TIME lnCurrentTime);
	virtual void ChangeSummary(const CStatisticSummary & summary) {}

	virtual void SetEngine(CHostEngine * pEngine);
	virtual SIM_TIME GetSimTime() const;
	virtual void EngineRegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds);
	virtual void EngineWriteLog(const CString & strLog);

	CHostEngine * GetEngine() { return m_pEngine; }

private:
	CHostEngine * m_pEngine;
};

