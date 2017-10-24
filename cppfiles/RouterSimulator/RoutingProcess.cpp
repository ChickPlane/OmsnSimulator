#include "stdafx.h"
#include "RoutingProcess.h"
#include "HostEngine.h"

#include "RoutingProtocol.h"
#include "Host.h"
#include "Yell.h"
#include "Sentence.h"


CRoutingProcess::CRoutingProcess()
	: m_pProtocol(NULL)
	, m_nProcessID(-1)
{
}


CRoutingProcess::~CRoutingProcess()
{
}

void CRoutingProcess::SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol)
{
	m_nProcessID = nProcessID;
	m_pProtocol = pProtocol;
}

void CRoutingProcess::OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList)
{

}

void CRoutingProcess::OnEngineTimer(int nCommandId)
{

}

void CRoutingProcess::StartWork(BOOL bStart)
{

}

void CRoutingProcess::MarkProcessIdToSentences(CList<CSentence*> & SendingList) const
{
	POSITION pos = SendingList.GetHeadPosition();
	while (pos)
	{
		SendingList.GetNext(pos)->m_nProcessID = m_nProcessID;
	}
}

void CRoutingProcess::MarkProcessIdToSentences(CSentence* pSentence) const
{
	pSentence->m_nProcessID = m_nProcessID;
}

void CRoutingProcess::TransmitMessage(CRoutingProtocol * pTo, CYell * pMsg)
{
	if (m_pProtocol)
	{
		for (int i = 0; i < pMsg->m_nSentenceCount; ++i)
		{
			MarkProcessIdToSentences(pMsg->m_ppSentences[i]);
		}
		m_pProtocol->TransmitMessage(pTo, pMsg);
	}
	else
	{
		ASSERT(0);
	}
}

int CRoutingProcess::GetCarryingMessages(CMsgShowInfo & allMessages) const
{
	return 0;
}

int CRoutingProcess::GetInfoList(CList<CString> & ret) const
{
	return  0;
}

void CRoutingProcess::WriteLog(const CString & strLog)
{
	CString strWriteDown = _T("[Process] ") + strLog;
	EngineWriteLog(strWriteDown);
	OutputDebugString(_T("\n") + strWriteDown);
}