#include "stdafx.h"
#include "RoutingProtocol.h"
#include "HostEngine.h"
#include "Host.h"
#include "RoutingProcess.h"


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

void CRoutingProtocol::RecordNewPackage(CRoutingDataEnc & encData)
{
	encData.RecordHop(GetMsgHopInfo(MSG_HOP_STATE_SOURCE, HOP_INFO_TYPE_EVENT));
	EngineRecordPackage(encData.GetDataId(GetHost()), encData.m_Statistic, MSG_HOP_STATE_SOURCE);
}

void CRoutingProtocol::OnEngineTimer(int nCommandId)
{

}

void CRoutingProtocol::GetAllCarryingMessages(CList<CRoutingDataEnc> & ret) const
{
	ret.RemoveAll();
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		m_Processes[i]->GetCarryingMessages(ret);
	}
}

void CRoutingProtocol::OnDelivered(const CRoutingMsg * pMsgBase)
{
	if (!GetHost()->IsReceivedPackage(pMsgBase->m_pData))
	{
		CRoutingDataEnc * pEncData = pMsgBase->m_pData->GetDuplicate();
		pEncData->RecordHop(GetMsgHopInfo(MSG_HOP_STATE_DESTINATION, HOP_INFO_TYPE_EVENT));
		EngineRecordPackage(pEncData->GetDataId(GetHost()), pEncData->m_Statistic, MSG_HOP_STATE_DESTINATION);

		GetHost()->OnPackageArrived(*pEncData);
		delete pEncData;
		pEncData = NULL;
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

int CRoutingProtocol::GetInfoList(CList<CString> & ret)
{
	return 0;
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

void CRoutingProtocol::TransmitMessage(CRoutingProcess * pFromProcess, CRoutingProtocol * pTo, CRoutingMsg * pMsg)
{
	if (GetEngine())
	{
		pMsg->m_nProtocolType = GetProcessId(pFromProcess);
		ASSERT(pMsg->m_nProtocolType != INVALID_PROCESS_ID);
		pMsg->m_pData->RecordHop(GetMsgHopInfo(MSG_HOP_STATE_OTHERS, HOP_INFO_TYPE_DEPARTURE));
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


void CRoutingProtocol::WriteLog(const CString & strLog)
{
	CString strWriteDown = m_strLogPrefix + _T(" ") + strLog;
	EngineWriteLog(strWriteDown);
	OutputDebugString(_T("\n") + strWriteDown);
}

