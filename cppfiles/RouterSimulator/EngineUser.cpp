#include "stdafx.h"
#include "EngineUser.h"
#include "HostEngine.h"


CEngineUser::CEngineUser()
	: m_pEngine(NULL)
{
}


CEngineUser::~CEngineUser()
{
}

void CEngineUser::OnEngineTimer(int nCommandId)
{

}

void CEngineUser::OnEngineSpeedChanged()
{

}

void CEngineUser::OnEngineTimeChanged(SIM_TIME lnCurrentTime)
{

}

void CEngineUser::OnEngineMessageStatisticsChanged(const CStatisticsReport & report)
{

}

void CEngineUser::SetEngine(CHostEngine * pEngine)
{
	m_pEngine = pEngine;
}

void CEngineUser::EngineRecordPackage(int nDataId, const CMsgInsideInfo & msgInfo, int nState)
{
	m_pEngine->RecordPackageStateChange(nDataId, msgInfo, nState);
}

SIM_TIME CEngineUser::GetSimTime() const
{
	return m_pEngine->GetSimTime();
}

void CEngineUser::EngineRegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds)
{
	m_pEngine->RegisterTimer(nCommandId, pUser, lnLaterMilliseconds);
}

void CEngineUser::EngineWriteLog(const CString & strLog)
{
	m_pEngine->WriteLog(strLog);
}
