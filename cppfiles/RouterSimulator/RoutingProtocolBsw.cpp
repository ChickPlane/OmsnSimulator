#include "stdafx.h"
#include "RoutingProtocolBsw.h"
#include "TestRecordBsw.h"
#include "Yell.h"


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
}


CRoutingProtocolBsw::~CRoutingProtocolBsw()
{
	ResetAll();
}

void CRoutingProtocolBsw::CreateQueryMission(const CQueryMission * pMission)
{
 	CTestRecordBsw * pNewSessionRecord = new CTestRecordBsw();
	gm_allSessions.SetAt(pMission->m_nMissionId, pNewSessionRecord);
	SetMissionRecord(pMission->m_nMissionId, REC_ST_GENERATE);

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

COLORREF CRoutingProtocolBsw::GetInportantLevel() const
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
		strLog.Format(_T("\n%d Delievery to %d"), pPkg->m_pTestSession->m_nSessionId, GetHostId());
		WriteLog(strLog);

		SetMissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_REACH);
		SetMissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_REP_LEAVE);

		CPkgBswData * pNewReply = new CPkgBswData();
		pNewReply->m_pTestSession = pPkg->DeepCopySession();
		GetReplyProcess()->InitNewPackage(pNewReply, pPkg->m_uSenderId, pPkg->m_pTestSession->m_lnTimeOut);
		pNewReply->m_pSpeakTo = pPkg->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);

		TransmitSingleSentence(pNewReply);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		// The original requirestor.
		strLog.Format(_T("\%d nOri req %d"), pPkg->m_pTestSession->m_nSessionId, GetHostId());
		WriteLog(strLog);
		SetMissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_REP_RETURN);
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolBsw::OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		SetMissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_ST_FIRSTSEND);
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

void CRoutingProtocolBsw::ResetAll()
{
	int rKey;
	CTestRecordBsw * rValue;
	POSITION pos = gm_allSessions.GetStartPosition();
	while (pos)
	{
		gm_allSessions.GetNextAssoc(pos, rKey, rValue);
		delete rValue;
	}
	gm_allSessions.RemoveAll();
}

BOOL CRoutingProtocolBsw::SetMissionRecord(int nSessionId, int nEventId)
{
	CTestRecordBsw * pRecord = NULL;
	gm_allSessions.Lookup(nSessionId, pRecord);
	if (pRecord)
	{
		pRecord->m_lnTimes[nEventId] = GetSimTime();
		CString strLog;
		if (nEventId == REC_ST_GENERATE)
		{
			strLog.Format(_T("PKG %d: E%d T(%d)"), nSessionId, nEventId, pRecord->m_lnTimes[nEventId]);
		}
		else
		{
			strLog.Format(_T("PKG %d: E%d T(%d) TG(%d)"), nSessionId, nEventId, pRecord->m_lnTimes[nEventId], pRecord->m_lnTimes[nEventId] - pRecord->m_lnTimes[REC_ST_GENERATE]);
		}
		WriteLog(strLog);
		return TRUE;
	}
	return FALSE;
}

CMap<int, int, CTestRecordBsw *, CTestRecordBsw *> CRoutingProtocolBsw::gm_allSessions;