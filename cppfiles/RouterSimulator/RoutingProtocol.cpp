#include "stdafx.h"
#include "RoutingProtocol.h"
#include "HostEngine.h"
#include "Host.h"
#include "RoutingProcess.h"
#include "Yell.h"


CRoutingProtocol::CRoutingProtocol()
	: m_fCommunicationRadius(0)
{
}

CRoutingProtocol::~CRoutingProtocol()
{
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		delete m_Processes[i];
	}
}

double CRoutingProtocol::GetCommunicateRadius() const
{
	return m_fCommunicationRadius;
}

void CRoutingProtocol::SetEnvironment(CHost * pHost, CHostEngine * pEngine)
{
	m_pHost = pHost;
	SetEngine(pEngine);
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		m_Processes[i]->SetEnvironment(pHost, pEngine);
	}
}

void CRoutingProtocol::SetCommunicateRadius(double fCommunicationRadius)
{
	m_fCommunicationRadius = fCommunicationRadius;
}

void CRoutingProtocol::OnEngineTimer(int nCommandId)
{

}

void CRoutingProtocol::GetAllCarryingMessages(CMsgShowInfo & allMessages) const
{
	allMessages.m_Rows.RemoveAll();
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		m_Processes[i]->GetCarryingMessages(allMessages);
	}
}

void CRoutingProtocol::OnDelivered(const CQueryMission * pMission)
{
	if (!GetHost()->IsReceivedPackage(pMission))
	{
		GetHost()->OnPackageArrived(pMission);
	}
}

CHost * CRoutingProtocol::GetHost() const
{
	return m_pHost;
}

int CRoutingProtocol::GetHostId() const
{
	return GetHost()->m_nId;
}

CDoublePoint CRoutingProtocol::GetHostPostion(SIM_TIME lnTime) const
{
	return GetHost()->GetPosition(lnTime);
}

int CRoutingProtocol::AddProcess(CRoutingProcess * pProcess)
{
	int nProcessId = m_Processes.GetSize();
	pProcess->SetProtocol(this);
	pProcess->SetProcessID(nProcessId);
	m_Processes.Add(pProcess);
	return nProcessId;
}

int CRoutingProtocol::GetInfoList(CMsgShowInfo & allMessages) const
{
	return 0;
}

void CRoutingProtocol::Turn(BOOL bOn)
{

}

CMsgHopInfo CRoutingProtocol::GetMsgHopInfo(int nComment, HOP_INFO_TYPE eType) const
{
	CMsgHopInfo ret;
	ret.m_Time = GetSimTime();
	if (GetHost())
	{
		ret.m_Location = GetHost()->GetPosition(ret.m_Time);
	}
	ret.m_pProtocol = this;
	ret.m_nComment = nComment;
	ret.m_eInfoType = eType;
	return ret;
}

void CRoutingProtocol::TransmitMessage(CRoutingProcess * pFromProcess, CRoutingProtocol * pTo, CYell * pMsg)
{
	if (GetEngine())
	{
		GetEngine()->TransmitMessage(this, pTo, pMsg);
	}
	else
	{
		ASSERT(0);
	}
}

int CRoutingProtocol::GetProcessId(CRoutingProcess * pProcess)
{
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		if (m_Processes[i] == pProcess)
		{
			return i;
		}
	}
	return INVALID_PROCESS_ID;
}


int CRoutingProtocol::GetDebugNumber(int nParam)
{
	return 0;
}

void CRoutingProtocol::WriteLog(const CString & strLog)
{
	CString strWriteDown = m_strLogPrefix + _T(" ") + strLog;
	EngineWriteLog(strWriteDown);
	OutputDebugString(_T("\n") + strWriteDown);
}

CString CRoutingProtocol::GetDebugString() const
{
	return _T("");
}

