#include "stdafx.h"
#include "RoutingProcessBSW.h"
#include "HostEngine.h"
#include "RoutingProtocol.h"
#include "Host.h"
#include "RoutingMsgBSW.h"


CRoutingProcessBSW::CRoutingProcessBSW()
	: m_nCopyCount(1)
	, m_bWork(false)
	, m_bBswLogSwitch(true)
{
}


CRoutingProcessBSW::~CRoutingProcessBSW()
{
}

void CRoutingProcessBSW::SetCopyCount(int nCopyCount)
{
	m_nCopyCount = nCopyCount;
}

void CRoutingProcessBSW::GenerateMission(const CRoutingDataEnc & encData)
{
	CRoutingMsgBSW * pMsgBsw = new CRoutingMsgBSW();
	pMsgBsw->InitByData(encData);
	pMsgBsw->InitValues_Data(m_pProtocol, m_nCopyCount);
	m_DataList.AddTail(pMsgBsw);
	StartBinarySprayWaitProcess();
}

void CRoutingProcessBSW::OnReceivedMsg(const CRoutingMsg * pMsg)
{
	switch (pMsg->m_nMsgType)
	{
	case BSW_MSG_TYPE_HELLO:
	{
		OnReceivedHello(pMsg);
		break;
	}
	case BSW_MSG_TYPE_HELLO_ACK:
	{
		OnReceivedHelloAck(pMsg);
		break;
	}
	case BSW_MSG_TYPE_DATA:
	{
		OnReceivedData(pMsg);
		break;
	}
	default:
	{
		ASSERT(0);
	}
	}
}

void CRoutingProcessBSW::OnEngineTimer(int nCommandId)
{
	switch (nCommandId)
	{
	case BSW_TIMER_CMD_HELLO:
	{
		SendHello();
		ContinueBinarySprayWaitProcess();
		break;
	}
	}
}

void CRoutingProcessBSW::OnReceivedHello(const CRoutingMsg * pMsg)
{
	SendHelloAck(pMsg->m_pFrom);
}

void CRoutingProcessBSW::OnReceivedHelloAck(const CRoutingMsg * pMsg)
{
	SendData(pMsg->m_pFrom);
}

void CRoutingProcessBSW::OnReceivedData(const CRoutingMsg * pMsg)
{
	CRoutingMsgBSW * pNewMsg = new CRoutingMsgBSW(*(const CRoutingMsgBSW*)pMsg);
	pNewMsg->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_OTHERS, HOP_INFO_TYPE_ARRIVE));
	if (pNewMsg->m_pDestination == m_pProtocol)
	{
		m_pProtocol->OnDelivered(pNewMsg);
		delete pNewMsg;
	}
	else
	{
		CRoutingMsgBSW * pMsgRecord = (CRoutingMsgBSW*)CheckDuplicateInDataList(pNewMsg->m_nBswId);
		if (pMsgRecord)
		{
			pMsgRecord->MergeMessage(*pNewMsg);
			delete pNewMsg;
		}
		else
		{
			m_DataList.AddTail(pNewMsg);
			StartBinarySprayWaitProcess();
		}
	}
}

void CRoutingProcessBSW::SendHello()
{
	CRoutingMsgBSW * pMsg = new CRoutingMsgBSW();
	pMsg->InitValues_Hello(m_pProtocol);
	TransmitMessage(NULL, pMsg);
}

void CRoutingProcessBSW::SendHelloAck(CRoutingProtocol * pTo)
{
	CRoutingMsgBSW * pMsg = new CRoutingMsgBSW();
	pMsg->InitValues_HelloAck(m_pProtocol, pTo);
	TransmitMessage(pTo, pMsg);
}

void CRoutingProcessBSW::SendData(CRoutingProtocol * pNext)
{
	POSITION pos = m_DataList.GetHeadPosition(), posLast;
	while (pos)
	{
		bool bSent = true;
		bool bDeleteMsg = false;
		posLast = pos;
		CRoutingMsgBSW * pMsg = m_DataList.GetNext(pos);

		if (pNext == pMsg->m_pDestination)
		{
			bDeleteMsg = true;
		}
		else
		{
			if (pMsg->m_nCopyCount == 1)
			{
				bSent = false;
			}
			if (pMsg->IsKnownNode(pNext))
			{
				bSent = false;
			}
		}

		if (bSent)
		{
			if (pMsg->m_KnownNodes.GetSize() == 1)
			{
				pMsg->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_BSW_BEGIN, HOP_INFO_TYPE_EVENT));
				m_pProtocol->EngineRecordPackage(pMsg->m_pData->GetDataId(m_pHost), pMsg->m_pData->m_Statistic, MSG_HOP_STATE_BSW_BEGIN);
			}
			pMsg->UpdateRecordMsg(pNext);
			CRoutingMsgBSW * pNew = new CRoutingMsgBSW(*pMsg);
			pNew->SetSendValues_Data(m_pProtocol, pNext);
			TransmitMessage(pNext, pNew);
		}

		if (bDeleteMsg)
		{
			delete pMsg;
			m_DataList.RemoveAt(posLast);
		}
	}
}

void CRoutingProcessBSW::CleanTimeOutData()
{
	SIM_TIME lnTime = GetSimTime();
	POSITION pos = m_DataList.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CRoutingMsgBSW * pMsg = m_DataList.GetNext(pos);
		if (pMsg->m_lnTimeOut < lnTime)
		{
			delete pMsg;
			m_DataList.RemoveAt(posLast);
		}
	}
}

void CRoutingProcessBSW::WriteLog(const CString & strLog)
{
	if (m_bBswLogSwitch)
	{
		CString strWriteDown = _T("[BSW] ") + strLog;
		EngineWriteLog(strWriteDown);
		OutputDebugString(_T("\n") + strWriteDown);
	}
}

int CRoutingProcessBSW::GetCarryingMessages(CList<CRoutingDataEnc> & appendTo) const
{
	POSITION pos = m_DataList.GetHeadPosition();
	while (pos)
	{
		CRoutingMsgBSW * pBswMsg = m_DataList.GetNext(pos);
		appendTo.AddTail(*pBswMsg->m_pData);
	}
	return m_DataList.GetSize();
}

void CRoutingProcessBSW::StartBinarySprayWaitProcess()
{
	if (m_bWork == false)
	{
		m_bWork = true;
		SendHello();
		EngineRegisterTimer(BSW_TIMER_CMD_HELLO, this, 1000);
	}
}

void CRoutingProcessBSW::ContinueBinarySprayWaitProcess()
{
	CleanTimeOutData();
	if (m_DataList.GetSize() > 0)
	{
		EngineRegisterTimer(BSW_TIMER_CMD_HELLO, this, 1000);
		ASSERT(m_bWork == true);
	}
	else
	{
		m_bWork = false;
	}
}

CRoutingMsgBSW * CRoutingProcessBSW::CheckDuplicateInDataList(int nBswId)
{
	POSITION pos = m_DataList.GetHeadPosition();
	while (pos)
	{
		CRoutingMsgBSW * pMsgRecord = m_DataList.GetNext(pos);
		if (nBswId == pMsgRecord->m_nBswId)
		{
			return pMsgRecord;
		}
	}
	return NULL;
}

int CRoutingProcessBSW::GetMsgCount() const
{
	return m_DataList.GetSize();
}
