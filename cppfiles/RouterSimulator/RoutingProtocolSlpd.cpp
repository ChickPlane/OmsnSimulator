#include "stdafx.h"
#include "RoutingProtocolSlpd.h"
#include "TrustValue.h"
#include "MobileSocialNetworkHost.h"
#include "Yell.h"
#include "HostEngine.h"


CRoutingProtocolSlpd::CRoutingProtocolSlpd()
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
	CRoutingProcessSlpd * pSlpd = new CRoutingProcessSlpd();
	pSlpd->SetProcessUser(this);
	m_nSlpdProcessId = AddProcess(pSlpd);
}


CRoutingProtocolSlpd::~CRoutingProtocolSlpd()
{
}

void CRoutingProtocolSlpd::CreateQueryMission(const CQueryMission * pMission)
{
	CTestRecordSlpd * pNewSessionRecord = new CTestRecordSlpd();
	gm_allSessions.SetAt(pMission->m_nMissionId, pNewSessionRecord);

	CPkgSlpd * pPkgSlpd = new CPkgSlpd();
	pPkgSlpd->m_pSession = new CTestSession();
	pPkgSlpd->m_pSession->m_lnTimeOut = pMission->m_lnTimeOut;
	pPkgSlpd->m_pSession->m_nSessionId = pMission->m_nMissionId;
	pPkgSlpd->m_RecverId = pMission->m_RecverId;
	GetSlpdProcess()->CreateQueryMission(pPkgSlpd);

	SetMissionRecord(pMission->m_nMissionId, REC_SLPD_ST_GENERATE);
}

void CRoutingProtocolSlpd::SetStaticParameters(int nK, double fTrust)
{
	gm_fTrust = fTrust;
	CRoutingProcessSlpd::SetParameters(nK);
}

void CRoutingProtocolSlpd::SetLocalParameters(int nBswCopyCount)
{
	GetQueryProcess()->SetCopyCount(nBswCopyCount);
	GetReplyProcess()->SetCopyCount(nBswCopyCount);
}

COLORREF CRoutingProtocolSlpd::GetImportantLevel() const
{
	return RGB(0, 0, 150);
}

int CRoutingProtocolSlpd::GetInfoList(CMsgShowInfo & allMessages) const
{
	CMsgShowRow row;
	row.m_Item0.Format(_T("Obfus %d"), GetSlpdProcess()->GetObfuscationCount());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Query %d"), GetQueryProcess()->GetDataMapSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Reply %d"), GetReplyProcess()->GetDataMapSize());
	allMessages.m_Rows.AddTail(row);
	return 0;
}

void CRoutingProtocolSlpd::Turn(BOOL bOn)
{
	GetHelloProcess()->StartWork(TRUE);
}

int CRoutingProtocolSlpd::GetDebugNumber(int nParam)
{
	return 0;
}

CString CRoutingProtocolSlpd::GetDebugString() const
{
	return _T("");
}

void CRoutingProtocolSlpd::OnEngineConnection(const CList<CHostGui> & m_Hosts)
{
	GetHelloProcess()->OnSomeoneNearby(m_Hosts);
}

BOOL CRoutingProtocolSlpd::IsTrustful(CRoutingProcessSlpd * pCallBy, const CRoutingProtocol * pOther) const
{
	CTrustValue fTrustValue;
	if (((CMobileSocialNetworkHost*)GetHost())->FindTrust(pOther->GetHostId(), fTrustValue))
	{
		if (fTrustValue > gm_fTrust)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CRoutingProtocolSlpd::OnNewSlpdPseudoOver(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg)
{
	// Session in the query
	CTestSessionBsw * pTestSession = new CTestSessionBsw();
	pTestSession->InitSession(pPkg->m_pSession->m_nSessionId);
	pTestSession->m_lnTimeOut = pPkg->m_pSession->m_lnTimeOut;

	CPkgBswData * pNewQuery = new CPkgBswData();
	pNewQuery->m_pTestSession = pTestSession;

	GetQueryProcess()->InitNewPackage(pNewQuery, pPkg->m_RecverId, pTestSession->m_lnTimeOut);
	GetQueryProcess()->InsertToDataMap(pNewQuery);
}

void CRoutingProtocolSlpd::OnFirstSlpdObfuscationForward(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg)
{
	SetMissionRecord(pPkg->m_pSession->m_nSessionId, REC_SLPD_ST_FIRSTSEND);
}

void CRoutingProtocolSlpd::OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg)
{
	CList<CSentence *> sendingList;

	// SLPD
	GetSlpdProcess()->OnEncounterUser(pPkg->m_pSender, sendingList);

	// BSW QUERY
	GetQueryProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pPkg);
	// BSW REPLY
	GetReplyProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pPkg);

	CYell * pNewYell = new CYell();
	CRoutingProtocol * pTo = pNewYell->SetSentences(sendingList);
	TransmitMessage(pTo, pNewYell);
}

CPkgAck * CRoutingProtocolSlpd::GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo)
{
	CPkgSlpdAck * pNewAck = new CPkgSlpdAck();
	pNewAck->m_pSpeakTo = pTo;
	pNewAck->SetIds(GetSlpdProcess()->GetPseudonymList());
	return pNewAck;
}

BOOL CRoutingProtocolSlpd::IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger)
{
	if (pCallBy == GetQueryProcess())
	{
		return pPkg->m_uReceiverId == pTheOther->GetHostId();
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgSlpdReply * pReply = (CPkgSlpdReply *)pPkg;
		if (pReply->m_bIsPseudonym)
		{
			CPkgSlpdAck * pAck = (CPkgSlpdAck *)pTriger;
			if (pAck->IsInId(pReply->m_uReceiverId))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return pReply->m_uReceiverId == pTheOther->GetHostId();
		}
	}
	ASSERT(0);
	return TRUE;
}

BOOL CRoutingProtocolSlpd::IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		return pPkg->m_uReceiverId == GetHostId();
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgSlpdReply * pReply = (CPkgSlpdReply *)pPkg;
		if (!pReply->m_bIsPseudonym)
		{
			return pReply->m_uReceiverId == GetHostId();
		}
		else
		{
			CSlpdUserAndPseudo forwardRecord;
			return GetSlpdProcess()->IsInPseudonymList(pReply->m_uReceiverId, FALSE, forwardRecord);
		}
	}
	ASSERT(0);
	return TRUE;
}

CPkgBswData * CRoutingProtocolSlpd::CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		return new CPkgBswData(*(CPkgBswData*)pPkg);
	}
	if (pCallBy == GetReplyProcess())
	{
		return new CPkgSlpdReply(*(CPkgSlpdReply*)pPkg);
	}
	ASSERT(0);
	return NULL;
}

void CRoutingProtocolSlpd::OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	CString strLog;
	if (pCallBy == GetQueryProcess())
	{
		// LBS REPLY
		const CPkgBswData * pQuery = pPkg;
		strLog.Format(_T("\nDelievery to %d"), GetHostId());
		WriteLog(strLog);

		SetMissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_SLPD_ST_REACH);

		if (!gm_bEnableLbsp)
		{
			return;
		}

		CPkgSlpdReply * pNewReply = LbsPrepareReply(pQuery);
		GetReplyProcess()->InitNewPackage(pNewReply);
		pNewReply->m_pSpeakTo = pQuery->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);

		TransmitSingleSentence(pNewReply);

		SetMissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_SLPD_ST_REP_LEAVE);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgSlpdReply * pReply = (CPkgSlpdReply *)pPkg;

		if (!pReply->m_bIsPseudonym)
		{
			// Is an agency
			CPkgSlpdReply * pNewReply = ForwardToOriginal(pReply);

			strLog.Format(_T("\nAgency %d To %d"), GetHostId(), pNewReply->GetReceiverId());
			WriteLog(strLog);
			return;
		}
		else
		{
			// The original requirestor.
			strLog.Format(_T("\nOri req %d"), GetHostId());
			WriteLog(strLog);
			SetMissionRecord(pReply->m_pTestSession->m_nSessionId, REC_SLPD_ST_REP_RETURN);
		}
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolSlpd::OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		SetMissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_SLPD_ST_OBFUSCATION_OVER);
	}
}

CPkgSlpdReply * CRoutingProtocolSlpd::ForwardToOriginal(const CPkgSlpdReply * pReply)
{
	CPkgSlpdReply * pRet = new CPkgSlpdReply(*pReply);
	CSlpdUserAndPseudo forwardRecord;
	if (!GetSlpdProcess()->IsInPseudonymList(pReply->m_bIsPseudonym, TRUE, forwardRecord))
	{
		ASSERT(0);
		return NULL;
	}
	pRet->m_uReceiverId = forwardRecord.m_lnUserId;
	return pRet;
}

BOOL CRoutingProtocolSlpd::SetMissionRecord(int nSessionId, int nEventId)
{
	CTestRecordSlpd * pRecord = NULL;
	gm_allSessions.Lookup(nSessionId, pRecord);
	if (pRecord)
	{
		pRecord->m_lnTimes[nEventId] = GetSimTime();
#ifdef DEBUG
		CString strLog;
		if (nEventId == REC_SLPD_ST_GENERATE)
		{
			strLog.Format(_T("PKG %d: E%d T(%d)"), nSessionId, nEventId, pRecord->m_lnTimes[nEventId]);
		}
		else
		{
			strLog.Format(_T("PKG %d: E%d T(%d) TG(%d)"), nSessionId, nEventId, pRecord->m_lnTimes[nEventId], pRecord->m_lnTimes[nEventId] - pRecord->m_lnTimes[REC_SLPD_ST_GENERATE]);
		}
		WriteLog(strLog);
#endif
		UpdateSummary();
		return TRUE;
	}
	return FALSE;
}

void CRoutingProtocolSlpd::UpdateSummary()
{
	CStatisticSummary & summary = GetEngine()->GetSummary();
	if (summary.m_RecentData.m_ProtocolRecords.GetSize() != REC_SLPD_ST_MAX)
	{
		summary.m_RecentData.m_ProtocolRecords.SetSize(REC_SLPD_ST_MAX);
	}

	for (int i = 0; i < REC_SLPD_ST_MAX; ++i)
	{
		summary.m_RecentData.m_ProtocolRecords[i] = 0;
	}

	POSITION pos = gm_allSessions.GetStartPosition();
	while (pos)
	{
		CTestRecordSlpd * pRecord = NULL;
		int nId = 0;
		gm_allSessions.GetNextAssoc(pos, nId, pRecord);
		for (int i = 0; i < REC_SLPD_ST_MAX; ++i)
		{
			if (pRecord->m_lnTimes[i] >= 0)
			{
				summary.m_RecentData.m_ProtocolRecords[i]++;
			}
		}
	}
	GetEngine()->ChangeSummary();
}

CPkgSlpdReply * CRoutingProtocolSlpd::LbsPrepareReply(const CPkgBswData * pQuery)
{
	CPkgSlpdReply * pNewReply = new CPkgSlpdReply();
	pNewReply->m_pTestSession = pQuery->DeepCopySession();
	pNewReply->m_bIsPseudonym = TRUE;
	pNewReply->m_uReceiverId = pQuery->m_uSenderId;
	pNewReply->m_lnTimeOut = pQuery->m_pTestSession->m_lnTimeOut;
	return pNewReply;
}

CRoutingProcessSlpd * CRoutingProtocolSlpd::GetSlpdProcess() const
{
	return (CRoutingProcessSlpd*)m_Processes[m_nSlpdProcessId];
}

CRoutingProcessBsw * CRoutingProtocolSlpd::GetQueryProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nQueryBswProcessId];
}

CRoutingProcessBsw * CRoutingProtocolSlpd::GetReplyProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nReplyBswProcessId];
}

CRoutingProcessHello * CRoutingProtocolSlpd::GetHelloProcess() const
{
	return (CRoutingProcessHello*)m_Processes[m_nHelloProcessId];
}

CMap<int, int, CTestRecordSlpd *, CTestRecordSlpd *> CRoutingProtocolSlpd::gm_allSessions;

double CRoutingProtocolSlpd::gm_fTrust = 1;
