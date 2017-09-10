#pragma once
#include "SimulatorCommon.h"
#include "StatisticsReport.h"
#include "MsgInsideInfo.h"

class CHostEngine;

class CEngineUser
{
public:
	CEngineUser();
	virtual ~CEngineUser();

	virtual void OnEngineTimer(int nCommandId) = 0;
	virtual void OnEngineSpeedChanged();
	virtual void OnEngineMessageStatisticsChanged(const CStatisticsReport & report);

	virtual void SetEngine(CHostEngine * pEngine);
	virtual void EngineRecordPackage(int nDataId, const CMsgInsideInfo & msgInfo, int nState);
	virtual SIM_TIME GetSimTime() const;
	virtual void EngineRegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds);
	virtual void EngineWriteLog(const CString & strLog);

	const CHostEngine * GetEngine() const;
	CHostEngine * GetEngine() { return m_pEngine; }

private:
	CHostEngine * m_pEngine;
};

