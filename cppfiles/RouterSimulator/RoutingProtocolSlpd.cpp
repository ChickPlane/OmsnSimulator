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
	m_nSessionRecordEntrySize = REC_SLPD_ST_MAX;
}


CRoutingProtocolSlpd::~CRoutingProtocolSlpd()
{
}

void CRoutingProtocolSlpd::CreateQueryMission(const CQueryMission * pMission)
{
	CPkgSlpd * pPkgSlpd = new CPkgSlpd();
	pPkgSlpd->m_pTestSession = new CTestSession();
	pPkgSlpd->m_pTestSession->m_lnTimeOut = pMission->m_lnTimeOut;
	pPkgSlpd->m_pTestSession->m_nSessionId = pMission->m_nMissionId;
	pPkgSlpd->m_RecverId = pMission->m_RecverId;
	GetSlpdProcess()->CreateQueryMission(pPkgSlpd);

	SetSissionRecord(pMission->m_nMissionId, REC_SLPD_ST_GENERATE);
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
	if (GetSlpdProcess()->GetObfuscationCount() > 0)
	{
		return RGB(255, 0, 0);
	}
	return RGB(50, 50, 150);
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

int CRoutingProtocolSlpd::GetCarryingPkgNumber(int nParam)
{
	switch (nParam)
	{
	case PROTOCOL_PKG_TYPE_SUPPLY:
	{
		return 0;
	}
	case PROTOCOL_PKG_TYPE_QUERY:
	{
		return GetQueryProcess()->GetDataMapSize() + GetSlpdProcess()->GetObfuscationCount();
	}
	case PROTOCOL_PKG_TYPE_REPLY:
	{
		return GetReplyProcess()->GetDataMapSize();
	}
	}
}

CString CRoutingProtocolSlpd::GetDebugString() const
{
	return _T("");
}

void CRoutingProtocolSlpd::OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev)
{
	GetHelloProcess()->OnSomeoneNearby(bAnyOneNearby, bDifferentFromPrev);
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
	*(CTestSession*)pTestSession = *pPkg->m_pTestSession;
	pTestSession->InitSession(pPkg->m_pTestSession->m_nSessionId);
	pTestSession->m_lnTimeOut = pPkg->m_pTestSession->m_lnTimeOut;

	CPkgBswData * pNewQuery = new CPkgBswData();
	pNewQuery->m_pTestSession = pTestSession;

	GetQueryProcess()->InitNewPackage(pNewQuery, pPkg->m_RecverId, pTestSession->m_lnTimeOut);
	pNewQuery->m_uSenderId = pPkg->m_nPseudonym;
	GetQueryProcess()->InsertToDataMap(pNewQuery);
}

void CRoutingProtocolSlpd::OnFirstSlpdObfuscationForward(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg)
{
	SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_SLPD_ST_FIRSTSEND);
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

		SetSissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_SLPD_ST_REACH);
		SetSissionForwardNumber(pQuery->m_pTestSession->m_nSessionId, pQuery->m_pTestSession->m_nForwardNumber);

		if (!gm_bEnableLbsp)
		{
			return;
		}

		CPkgSlpdReply * pNewReply = LbsPrepareReply(pQuery);
		GetReplyProcess()->InitNewPackage(pNewReply);
		pNewReply->m_pSpeakTo = pQuery->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);
		pNewReply->m_pTestSession->m_bInStatistic = FALSE;

		TransmitSingleSentence(pNewReply);

		SetSissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_SLPD_ST_REP_LEAVE);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgSlpdReply * pReply = (CPkgSlpdReply *)pPkg;

		if (pReply->m_bIsPseudonym)
		{
			// Is an agency
			CPkgSlpdReply * pNewReply = ForwardToOriginal(pReply);
			GetReplyProcess()->InsertToDataMap(pNewReply);
			return;
		}
		else
		{
			// The original requirestor.
			if (SetSissionRecord(pReply->m_pTestSession->m_nSessionId, REC_SLPD_ST_REP_RETURN))
			{
				strLog.Format(_T("\nReturn to original %d"), GetHostId());
				WriteLog(strLog);
			}
		}
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolSlpd::OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_SLPD_ST_OBFUSCATION_OVER);
	}
}

CPkgSlpdReply * CRoutingProtocolSlpd::ForwardToOriginal(const CPkgSlpdReply * pReply)
{
	CPkgSlpdReply * pRet = new CPkgSlpdReply(*pReply);
	GetReplyProcess()->InitNewPackage(pRet);
	CSlpdUserAndPseudo forwardRecord;
	if (!GetSlpdProcess()->IsInPseudonymList(pReply->m_uReceiverId, TRUE, forwardRecord))
	{
		ASSERT(0);
		return NULL;
	}
	pRet->m_uReceiverId = forwardRecord.m_lnUserId;
	pRet->m_bIsPseudonym = FALSE;
	GetReplyProcess()->MarkProcessIdToSentences(pRet);
	return pRet;
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

double CRoutingProtocolSlpd::gm_fTrust = 1;
