#include "stdafx.h"
#include "RoutingProcessPmhns.h"
#include "RoutingMsgPmhns.h"
#include "Host.h"
#include "HostEngine.h"



CRoutingProcessPmhns::CRoutingProcessPmhns()
	: m_bDsrLogSwitch(false)
	, m_nReqId(0)
	, m_pUser(NULL)
{
}


CRoutingProcessPmhns::~CRoutingProcessPmhns()
{
}

void CRoutingProcessPmhns::OnReceivedMsg(const CRoutingMsg * pMsg)
{
	if (!pMsg->IsBelongTo(m_pProtocol))
	{
		return;
	}

	switch (pMsg->m_nMsgType)
	{
	case PMHNS_MSG_TYPE_DATA:
	{
		OnGetData(pMsg);
		break;
	}
	case PMHNS_MSG_TYPE_RREQ:
	{
		OnGetRReq(pMsg);
		break;
	}
	case PMHNS_MSG_TYPE_RREP:
	{
		OnGetRRep(pMsg);
		break;
	}
	default:
	{
		ASSERT(0);
	}
	}
}

void CRoutingProcessPmhns::OnEngineTimer(int nCommandId)
{
	switch (nCommandId)
	{
	}
}

void CRoutingProcessPmhns::OnGetRReq(const CRoutingMsg * pMsg)
{
	CRoutingMsgPmhns * pDsrMsg = (CRoutingMsgPmhns*)pMsg;
	if (!RecordReq(pMsg->m_pSource, pDsrMsg->m_nReqId))
	{
		return;
	}

	int nFromId = pMsg->m_pSource->GetHost()->m_nId;
	int nToId = m_pProtocol->GetHost()->m_nId;
	CString strOut;
	strOut.Format(_T("Receive QQQ(%X) From %d To %d"), pDsrMsg->m_nReqId, nFromId, nToId);
	WriteLog(strOut);

	SIM_TIME simTime = GetSimTime();
	bool bInRequestArea = pDsrMsg->IsRequestArea(m_pProtocol->GetHost()->GetPosition(simTime));
	if (bInRequestArea)
	{
		pDsrMsg = new CRoutingMsgPmhns(*(CRoutingMsgPmhns*)pMsg);
		WriteLog(_T("Retransmission RReq"));
		pDsrMsg->SetRetransmissionRReq(m_pProtocol);
		TransmitMessage(NULL, pDsrMsg);

		pDsrMsg = new CRoutingMsgPmhns(*(CRoutingMsgPmhns*)pMsg);
		WriteLog(_T("Send RRep"));
		pDsrMsg->InitValues_RRep(m_pProtocol);
		TransmitMessage(NULL, pDsrMsg);
	}
	
}

void CRoutingProcessPmhns::OnGetRRep(const CRoutingMsg * pMsg)
{
	if (m_bDsrLogSwitch)
	{
		int nFromId = pMsg->m_pFrom->GetHost()->m_nId;
		int nToId = m_pProtocol->GetHost()->m_nId;
		CString strOut;
		strOut.Format(_T("Receive PPP From %d To %d"), nFromId, nToId);
		WriteLog(strOut);
	}
	if (pMsg->m_pDestination == m_pProtocol)
	{
		CRoutingMsgPmhns * pDsrMsg = (CRoutingMsgPmhns*)pMsg;
		m_pUser->OnGetPmhnsRep(pDsrMsg->m_ReqPath);
	}
	else
	{
		CRoutingMsgPmhns * pDsrMsg = new CRoutingMsgPmhns(*(CRoutingMsgPmhns*)pMsg);
		pDsrMsg->SetRetransmissionRRep(m_pProtocol);
		WriteLog(_T("Retransmission RRep"));
		TransmitMessage(NULL, pDsrMsg);
	}
}

void CRoutingProcessPmhns::OnGetData(const CRoutingMsg * pMsg)
{
	if (m_bDsrLogSwitch)
	{
		int nFromId = pMsg->m_pSource->GetHost()->m_nId;
		int nToId = m_pProtocol->GetHost()->m_nId;
		CString strOut;
		strOut.Format(_T("Receive DAT From %d To %d"), nFromId, nToId);
		WriteLog(strOut);
	}
	CRoutingMsgPmhns * pDsrMsg = new CRoutingMsgPmhns(*(CRoutingMsgPmhns*)pMsg);
	pDsrMsg->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_OTHERS, HOP_INFO_TYPE_ARRIVE));
	if (pMsg->m_pDestination == m_pProtocol)
	{
		m_pUser->OnGetPmhnsData(pDsrMsg);
		delete pDsrMsg;
	}
	else
	{
		pDsrMsg->SetRetransmissionData(m_pProtocol);
		WriteLog(_T("Retransmission Data"));
		TransmitMessage(NULL, pDsrMsg);
	}
}

void CRoutingProcessPmhns::GetRoutings()
{
	CRoutingMsgPmhns * pNewReq = new CRoutingMsgPmhns();
	RecordReq(m_pProtocol, m_nReqId);
	pNewReq->InitValues_RReq(m_pProtocol, m_nReqId++);
	TransmitMessage(NULL, pNewReq);
}

void CRoutingProcessPmhns::SendDataPackage(const CList<CRoutingProtocol*> & path, const CRoutingDataEnc & encData)
{
	CRoutingMsgPmhns * pDsrMsg = new CRoutingMsgPmhns();
	pDsrMsg->InitByData(encData);
	pDsrMsg->InitValues_Data(path);
	WriteLog(_T("Retransmission Data"));
	TransmitMessage(NULL, pDsrMsg);
}

void CRoutingProcessPmhns::GenerateMission(const CRoutingDataEnc & encData)
{

}

void CRoutingProcessPmhns::WriteLog(const CString & strLog)
{
	if (m_bDsrLogSwitch)
	{
		CString strWriteDown = _T("[PMHNS] ") + strLog;
		EngineWriteLog(strWriteDown);
		OutputDebugString(_T("\n") + strWriteDown);
	}
}

bool CRoutingProcessPmhns::IsNewReq(CRoutingProtocol * pProtocol, int nReqId) const
{
	int nValue;
	if (m_ReqMap.Lookup(pProtocol, nValue))
	{
		if (nValue >= nReqId)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool CRoutingProcessPmhns::RecordReq(CRoutingProtocol * pProtocol, int nReqId)
{
	if (IsNewReq(pProtocol, nReqId))
	{
		m_ReqMap[pProtocol] = nReqId;
		return true;
	}
	else
	{
		return false;
	}
}

int CRoutingProcessPmhns::GetCarryingMessages(CList<CRoutingDataEnc> & appendTo) const
{
	return 0;
}

void CRoutingUserProcessPmhns::OnGetPmhnsRep(const CList<CRoutingProtocol*> & path)
{

}

void CRoutingUserProcessPmhns::OnGetPmhnsData(const CRoutingMsg * pMsg)
{

}
