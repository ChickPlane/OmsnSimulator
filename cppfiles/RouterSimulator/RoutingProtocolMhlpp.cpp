#include "stdafx.h"
#include "RoutingProtocolMhlpp.h"
#include "TrustValue.h"
#include "MobileSocialNetworkHost.h"
#include "Yell.h"
#include "HostEngine.h"


CRoutingProtocolMhlpp::CRoutingProtocolMhlpp()
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
	CRoutingProcessMultiHop * pMh = new CRoutingProcessMultiHop();
	pMh->SetProcessUser(this);
	m_nMhProcessId = AddProcess(pMh);
	m_nSessionRecordEntrySize = REC_MHLPP_ST_MAX;
}


CRoutingProtocolMhlpp::~CRoutingProtocolMhlpp()
{
}

void CRoutingProtocolMhlpp::CreateQueryMission(const CQueryMission * pMission)
{
	SetSissionRecord(pMission->m_nMissionId, REC_MHLPP_ST_GENERATE);
	CPkgMhlpp NewPkg;
	NewPkg.m_pTestSession = new CTestSession();
	NewPkg.m_pTestSession->m_lnTimeOut = pMission->m_lnTimeOut;
	NewPkg.m_pTestSession->m_nSessionId = pMission->m_nMissionId;

	NewPkg.m_lnTimeout = pMission->m_lnTimeOut;
	NewPkg.m_RecverId = pMission->m_RecverId;
	NewPkg.m_nPseudonym = ++gm_nPseudonymMax;
	NewPkg.m_OriginalRequesterId = GetHostId();

	CTimeOutPair<CPkgMhlpp> pairNewPkg;
	pairNewPkg.m_lnTimeOut = pMission->m_lnTimeOut;
	pairNewPkg.m_Value = NewPkg;
	CTimeOutPair<CPkgMhlpp>::InsertToTimeoutPairList(pairNewPkg, m_WaitingList);
}

void CRoutingProtocolMhlpp::SetStaticParameters(double fTrust, double fObfuscationRadius)
{
	gm_fTrust = fTrust;
	gm_fObfuscationRadius = fObfuscationRadius;
}

void CRoutingProtocolMhlpp::SetLocalParameters(int nBswCopyCount)
{
	GetQueryProcess()->SetCopyCount(nBswCopyCount);
	GetReplyProcess()->SetCopyCount(nBswCopyCount);
}

COLORREF CRoutingProtocolMhlpp::GetImportantLevel() const
{
	int nR = GetQueryProcess()->GetDataMapSize();
	nR = 255 * nR / 10;
	if (nR > 255)
	{
		nR = 255;
	}
	return RGB(nR, 0, 0);
}

int CRoutingProtocolMhlpp::GetInfoList(CMsgShowInfo & allMessages) const
{
	CMsgShowRow row;
	row.m_Item0.Format(_T("Query %d"), GetQueryProcess()->GetDataMapSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Reply %d"), GetReplyProcess()->GetDataMapSize());
	allMessages.m_Rows.AddTail(row);
	return 0;
}

void CRoutingProtocolMhlpp::Turn(BOOL bOn)
{
	GetHelloProcess()->StartWork(TRUE);
}

void CRoutingProtocolMhlpp::OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev)
{
	GetHelloProcess()->OnSomeoneNearby(bAnyOneNearby, bDifferentFromPrev);
	if (bAnyOneNearby)
	{
		if (bDifferentFromPrev)
		{
			POSITION pos = m_WaitingList.GetHeadPosition(), posOld;
			while (pos)
			{
				posOld = pos;
				CPkgMhlpp & testing = m_WaitingList.GetNext(pos).m_Value;
				if (TryToFinishObfuscation(testing))
				{
					m_WaitingList.RemoveAt(posOld);
				}
			}
		}
		OnMulNeighbourDifferent(GetHost()->GetRecentReport());
	}
}

CRoutingProtocol * CRoutingProtocolMhlpp::GetNextHop(CRoutingProcessMultiHop * pCallBy, USERID nDestinationId)
{
	const CMsgCntJudgeReceiverReport* pRecentReport = GetHost()->GetRecentReport();
	if (!pRecentReport)
	{
		return NULL;
	}
	CHost * pNextHop = pRecentReport->m_ArrItems[GetHostId()].GetNextHop(nDestinationId);
	if (!pNextHop)
	{
		return NULL;
	}
	return pNextHop->m_pProtocol;
}

CPkgMultiHop * CRoutingProtocolMhlpp::GetMultiHopDataCopy(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg)
{
	return new CPkgMhlpp(*(CPkgMhlpp*)pPkg);
}

void CRoutingProtocolMhlpp::OnMultiHopMsgArrived(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg)
{
	CPkgMhlpp RecvedObfus(*(CPkgMhlpp*)pPkg);
	CTimeOutPair<CMhlppUserAndPseudo> newPseudoPair;
	newPseudoPair.m_lnTimeOut = RecvedObfus.m_lnTimeout;
	newPseudoPair.m_Value.m_lnPseudonym = RecvedObfus.m_nPseudonym;
	newPseudoPair.m_Value.m_lnUserId = RecvedObfus.m_OriginalRequesterId;
	CTimeOutPair<CMhlppUserAndPseudo>::InsertToTimeoutPairList(newPseudoPair, m_Pseudonyms);

	if (!TryToFinishObfuscation(RecvedObfus))
	{
		CTimeOutPair<CPkgMhlpp> newPair;
		newPair.m_lnTimeOut = RecvedObfus.m_lnTimeout;
		newPair.m_Value = RecvedObfus;
		CTimeOutPair<CPkgMhlpp>::InsertToTimeoutPairList(newPair, m_WaitingList);
	}

}

void CRoutingProtocolMhlpp::OnMulNeighbourDifferent(const CMsgCntJudgeReceiverReport* pWholeReport)
{
	int nMyId = GetHostId();
	const CReceiverReportItem & myItem = pWholeReport->m_ArrItems[nMyId];

	BOOL bCanSendObfus = FALSE;
	CHost * pNextHop = NULL;
	for (int i = 0; i < myItem.m_nNeighbourCount; ++i)
	{
		int nNeighId = myItem.m_pAllNeighbours[i];
		if (IsTrustful(nNeighId))
		{
			bCanSendObfus = TRUE;
			pNextHop = myItem.GetNextHop(nNeighId);
		}
	}
	if (!bCanSendObfus)
	{
		return;
	}
	POSITION pos = m_WaitingList.GetHeadPosition(), posOld;
	while (pos)
	{
		posOld = pos;
		CPkgMhlpp & testing = m_WaitingList.GetNext(pos).m_Value;
		if (testing.m_bInitialed == FALSE || IsFartherThanMe(testing, pNextHop))
		{
			if (testing.m_bInitialed == FALSE)
			{
				SetSissionRecord(testing.m_pTestSession->m_nSessionId, REC_MHLPP_ST_FIRSTSEND);
			}
			testing.m_nToId = pNextHop->m_nId;
			testing.m_OriginalPosition = GetHostPostion(GetSimTime());
			testing.m_bInitialed = TRUE;
			GetMhProcess()->SendPkgToMultiHopHost(&testing);
			CString strOut;
			strOut.Format(_T("\n[MH] %d : %d -> %d"), testing.m_pTestSession->m_nSessionId, GetHostId(), pNextHop->m_nId);
			OutputDebugString(strOut);
			m_WaitingList.RemoveAt(posOld);
		}
	}
}

void CRoutingProtocolMhlpp::OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg)
{
	CList<CSentence *> sendingList;

	// BSW QUERY
	GetQueryProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pPkg);
	// BSW REPLY
	GetReplyProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pPkg);

	CYell * pNewYell = new CYell();
	CRoutingProtocol * pTo = pNewYell->SetSentences(sendingList);
	TransmitMessage(pTo, pNewYell);
}

CPkgAck * CRoutingProtocolMhlpp::GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo)
{
	CPkgMhlppAck * pNewAck = new CPkgMhlppAck();
	pNewAck->m_pSpeakTo = pTo;
	pNewAck->SetIds(m_Pseudonyms);
	return pNewAck;
}

BOOL CRoutingProtocolMhlpp::IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger)
{
	if (pCallBy == GetQueryProcess())
	{
		return pPkg->m_uReceiverId == pTheOther->GetHostId();
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgMhlppReply * pReply = (CPkgMhlppReply *)pPkg;
		if (pReply->m_bIsPseudonym)
		{
			CPkgMhlppAck * pAck = (CPkgMhlppAck *)pTriger;
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

BOOL CRoutingProtocolMhlpp::IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		return pPkg->m_uReceiverId == GetHostId();
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgMhlppReply * pReply = (CPkgMhlppReply *)pPkg;
		if (!pReply->m_bIsPseudonym)
		{
			return pReply->m_uReceiverId == GetHostId();
		}
		else
		{
			CMhlppUserAndPseudo forwardRecord;
			return IsInPseudonymList(pReply->m_uReceiverId, forwardRecord);
		}
	}
	ASSERT(0);
	return TRUE;
}

CPkgBswData * CRoutingProtocolMhlpp::CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		return new CPkgBswData(*(CPkgBswData*)pPkg);
	}
	if (pCallBy == GetReplyProcess())
	{
		return new CPkgMhlppReply(*(CPkgMhlppReply*)pPkg);
	}
	ASSERT(0);
	return NULL;
}

void CRoutingProtocolMhlpp::OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	CString strLog;
	if (pCallBy == GetQueryProcess())
	{
		// LBS REPLY
		const CPkgBswData * pQuery = pPkg;
		strLog.Format(_T("\nDelievery to %d"), GetHostId());
		WriteLog(strLog);

		SetSissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_MHLPP_ST_REACH);
		SetSissionForwardNumber(pQuery->m_pTestSession->m_nSessionId, pQuery->m_pTestSession->m_nForwardNumber);

		if (!gm_bEnableLbsp)
		{
			return;
		}

		CPkgMhlppReply * pNewReply = LbsPrepareReply(pQuery);
		GetReplyProcess()->InitNewPackage(pNewReply);
		pNewReply->m_pSpeakTo = pQuery->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);
		pNewReply->m_pTestSession->m_bInStatistic = FALSE;

		TransmitSingleSentence(pNewReply);

		SetSissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_MHLPP_ST_REP_LEAVE);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgMhlppReply * pReply = (CPkgMhlppReply *)pPkg;

		if (pReply->m_bIsPseudonym)
		{
			// Is an agency
	
			CPkgMhlppReply * pNewReply = ForwardToOriginal(pReply);
			GetReplyProcess()->InsertToDataMap(pNewReply);
			return;
		}
		else
		{
			// The original requirestor.
			if (SetSissionRecord(pReply->m_pTestSession->m_nSessionId, REC_MHLPP_ST_REP_RETURN))
			{
				strLog.Format(_T("\nReturn to original %d"), GetHostId());
				WriteLog(strLog);
			}
		}
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolMhlpp::OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_MHLPP_ST_OBFUSCATION_OVER);
	}
}

CPkgMhlppReply * CRoutingProtocolMhlpp::ForwardToOriginal(const CPkgMhlppReply * pReply)
{
	CPkgMhlppReply * pRet = new CPkgMhlppReply(*pReply);
	GetReplyProcess()->InitNewPackage(pRet);
	CMhlppUserAndPseudo forwardRecord;
	if (!IsInPseudonymList(pReply->m_uReceiverId, forwardRecord))
	{
		ASSERT(0);
		return NULL;
	}
	pRet->m_uReceiverId = forwardRecord.m_lnUserId;
	pRet->m_bIsPseudonym = FALSE;
	GetReplyProcess()->MarkProcessIdToSentences(pRet);
	return pRet;
}

BOOL CRoutingProtocolMhlpp::IsTrustful(int nHostId) const
{
	CTrustValue fTrustValue;
	if (((CMobileSocialNetworkHost*)GetHost())->FindTrust(nHostId, fTrustValue))
	{
		if (fTrustValue > gm_fTrust)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CPkgMhlppReply * CRoutingProtocolMhlpp::LbsPrepareReply(const CPkgBswData * pQuery)
{
	CPkgMhlppReply * pNewReply = new CPkgMhlppReply();
	pNewReply->m_pTestSession = pQuery->DeepCopySession();
	pNewReply->m_bIsPseudonym = TRUE;
	pNewReply->m_uReceiverId = pQuery->m_uSenderId;
	pNewReply->m_lnTimeOut = pQuery->m_pTestSession->m_lnTimeOut;
	return pNewReply;
}

BOOL CRoutingProtocolMhlpp::TryToFinishObfuscation(const CPkgMhlpp & ObfuscatingPkg)
{
	if (ObfuscatingPkg.m_bInitialed == FALSE)
	{
		return FALSE;
	}
	CDoublePoint currentLocation = m_pHost->GetPosition(GetSimTime());
	double fDistance = CDoublePoint::GetDistance(currentLocation, ObfuscatingPkg.m_OriginalPosition);
	if (fDistance < gm_fObfuscationRadius)
	{
		return FALSE;
	}

	CTestSessionBsw * pTestSession = new CTestSessionBsw();
	*(CTestSession*)pTestSession = *ObfuscatingPkg.m_pTestSession;
	pTestSession->InitSession(ObfuscatingPkg.m_pTestSession->m_nSessionId);
	pTestSession->m_lnTimeOut = ObfuscatingPkg.m_pTestSession->m_lnTimeOut;

	CPkgBswData * pNewQuery = new CPkgBswData();
	pNewQuery->m_pTestSession = pTestSession;

	GetQueryProcess()->InitNewPackage(pNewQuery, ObfuscatingPkg.m_RecverId, pTestSession->m_lnTimeOut);
	pNewQuery->m_uSenderId = ObfuscatingPkg.m_nPseudonym;
	GetQueryProcess()->InsertToDataMap(pNewQuery);
	return TRUE;
}

BOOL CRoutingProtocolMhlpp::IsFartherThanMe(const CPkgMhlpp & ObfuscatingPkg, CHost * pTheOther)
{
	CDoublePoint currentLocation = m_pHost->GetPosition(GetSimTime());
	CDoublePoint theOtherLocation = pTheOther->GetPosition(GetSimTime());
	double fDistanceMy = CDoublePoint::GetDistance(currentLocation, ObfuscatingPkg.m_OriginalPosition);
	double fDistanceTheOther = CDoublePoint::GetDistance(theOtherLocation, ObfuscatingPkg.m_OriginalPosition);
	return fDistanceMy < fDistanceTheOther;
}

BOOL CRoutingProtocolMhlpp::IsInPseudonymList(USERID lnPseudonym, CMhlppUserAndPseudo & ret)
{
	POSITION pos = m_Pseudonyms.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CTimeOutPair<CMhlppUserAndPseudo> & tmpUP = m_Pseudonyms.GetNext(pos);
		if (tmpUP.m_Value.m_lnPseudonym == lnPseudonym)
		{
			ret = tmpUP.m_Value;
			return TRUE;
		}
	}
	return FALSE;
}

CRoutingProcessMultiHop * CRoutingProtocolMhlpp::GetMhProcess() const
{
	return (CRoutingProcessMultiHop*)m_Processes[m_nMhProcessId];
}

CRoutingProcessBsw * CRoutingProtocolMhlpp::GetQueryProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nQueryBswProcessId];
}

CRoutingProcessBsw * CRoutingProtocolMhlpp::GetReplyProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nReplyBswProcessId];
}

CRoutingProcessHello * CRoutingProtocolMhlpp::GetHelloProcess() const
{
	return (CRoutingProcessHello*)m_Processes[m_nHelloProcessId];
}

double CRoutingProtocolMhlpp::gm_fTrust = 1;

double CRoutingProtocolMhlpp::gm_fObfuscationRadius = 100;

int CRoutingProtocolMhlpp::gm_nPseudonymMax = 0;
