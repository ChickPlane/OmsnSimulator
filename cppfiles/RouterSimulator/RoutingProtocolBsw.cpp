#include "stdafx.h"
#include "RoutingProtocolBsw.h"
#include "TestRecordBsw.h"
#include "Yell.h"
#include "HostEngine.h"


CRoutingProtocolBsw::CRoutingProtocolBsw()
{
	CRoutingProcessHello * pHelloProcess = new CRoutingProcessHello();
	pHelloProcess->SetProcessUser(this);
	m_nHelloProcessId = AddProcess(pHelloProcess);
	CRoutingProcessBsw * pBswQuery = new CRoutingProcessBsw();
	pBswQuery->SetProcessUser(this);
	m_nQueryBswProcessId = AddProcess(pBswQuery);
	CRoutingProcessBsw * pBswReply = new CRoutingProcessBsw();
	pBswReply->SetProcessUser(this);
	m_nReplyBswProcessId = AddProcess(pBswReply);
	m_nSessionRecordEntrySize = REC_ST_MAX;
}


CRoutingProtocolBsw::~CRoutingProtocolBsw()
{
}

void CRoutingProtocolBsw::CreateQueryMission(const CQueryMission * pMission)
{
	SetSissionRecord(pMission->m_nMissionId, REC_ST_GENERATE);

	// Session in the query
	CTestSessionBsw * pTestSession = new CTestSessionBsw();
	pTestSession->InitSession(pMission->m_nMissionId);
	pTestSession->m_lnTimeOut = pMission->m_lnTimeOut;

	CPkgBswData * pNewQuery = new CPkgBswData();
	pNewQuery->m_pTestSession = pTestSession;

	GetQueryProcess()->InitNewPackage(pNewQuery, pMission->m_RecverId, pMission->m_lnTimeOut);
	GetQueryProcess()->InsertToDataMap(pNewQuery);

}


void CRoutingProtocolBsw::SetLocalParameters(int nBswCopyCount)
{
	GetQueryProcess()->SetCopyCount(nBswCopyCount);
	GetReplyProcess()->SetCopyCount(nBswCopyCount);
}

void CRoutingProtocolBsw::Turn(BOOL bOn)
{
	GetHelloProcess()->StartWork(TRUE);
}

int CRoutingProtocolBsw::GetCarryingPkgNumber(int nParam)
{
	switch (nParam)
	{
	case PROTOCOL_PKG_TYPE_SUPPLY:
	{
		return 0;
	}
	case PROTOCOL_PKG_TYPE_QUERY:
	{
		return GetQueryProcess()->GetDataMapSize();
	}
	case PROTOCOL_PKG_TYPE_REPLY:
	{
		return GetReplyProcess()->GetDataMapSize();
	}
	}
}

COLORREF CRoutingProtocolBsw::GetImportantLevel() const
{
	int nQ = GetQueryProcess()->GetDataMapSize();
	int nR = GetReplyProcess()->GetDataMapSize();
	if (nR > 0)
	{
		return RGB(255, 0, 0);
	}
	else
	{
		if (nQ > 0)
		{
			return RGB(0, 255, 0);
		}
		else
		{
			return RGB(50, 50, 50);
		}
	}
}

void CRoutingProtocolBsw::OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev)
{
	GetHelloProcess()->OnSomeoneNearby(bAnyOneNearby, bDifferentFromPrev);
}

void CRoutingProtocolBsw::OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg)
{
	CList<CSentence *> sendingList;

	GetQueryProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pPkg);
	GetReplyProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pPkg);

	CYell * pNewYell = new CYell();
	CRoutingProtocol * pTo = pNewYell->SetSentences(sendingList);
	TransmitMessage(pTo, pNewYell);
}

BOOL CRoutingProtocolBsw::IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	return pPkg->m_uReceiverId == GetHostId();
}

CPkgBswData * CRoutingProtocolBsw::CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	return new CPkgBswData(*(CPkgBswData*)pPkg);
}

void CRoutingProtocolBsw::OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	CString strLog;
	if (pCallBy == GetQueryProcess())
	{
		// LBS REPLY
		strLog.Format(_T("\nDelievery to %d"), GetHostId());
		WriteLog(strLog);

		SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_REACH);
		SetSissionForwardNumber(pPkg->m_pTestSession->m_nSessionId, pPkg->m_pTestSession->m_nForwardNumber);

		if (!gm_bEnableLbsp)
		{
			return;
		}

		SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_REP_LEAVE);

		CPkgBswData * pNewReply = new CPkgBswData();
		pNewReply->m_pTestSession = pPkg->DeepCopySession();
		GetReplyProcess()->InitNewPackage(pNewReply, pPkg->m_uSenderId, pPkg->m_pTestSession->m_lnTimeOut);
		pNewReply->m_pSpeakTo = pPkg->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);
		pNewReply->m_pTestSession->m_bInStatistic = FALSE;

		TransmitSingleSentence(pNewReply);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		// The original requirestor.
		if (SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_REP_RETURN))
		{
			strLog.Format(_T("\nReturn to original %d"), GetHostId());
			WriteLog(strLog);
		}
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolBsw::OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_FIRSTSEND);
	}
}

CRoutingProcessBsw * CRoutingProtocolBsw::GetQueryProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nQueryBswProcessId];
}

CRoutingProcessBsw * CRoutingProtocolBsw::GetReplyProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nReplyBswProcessId];
}

CRoutingProcessHello * CRoutingProtocolBsw::GetHelloProcess() const
{
	return (CRoutingProcessHello*)m_Processes[m_nHelloProcessId];
}
