#include "stdafx.h"
#include "RoutingProtocolAptCard.h"
#include "RoutingProcessAptCard.h"
#include "TrustValue.h"
#include "MobileSocialNetworkHost.h"
#include "SentenceAptCard.h"


CRoutingProtocolAptCard::CRoutingProtocolAptCard()
{
	CRoutingProcessHello * pHelloProcess = new CRoutingProcessHello();
	m_nHelloProcessId = AddProcess(pHelloProcess);
	CRoutingProcessBsw * pBswQuery = new CRoutingProcessBsw();
	m_nQueryBswProcessId = AddProcess(pBswQuery);
	CRoutingProcessBsw * pBswReply = new CRoutingProcessBsw();
	m_nReplyBswProcessId = AddProcess(pBswReply);
	CRoutingProcessAptCard * pAptCardProcess = new CRoutingProcessAptCard();
	m_nAptCardProcessId = AddProcess(pAptCardProcess);
}

CRoutingProtocolAptCard::~CRoutingProtocolAptCard()
{
	ResetAll();
}

void CRoutingProtocolAptCard::CreateQueryMission(const CQueryMission * pMission)
{
	CAppointmentCard * pSelectedAptCard = GetAptCardProcess()->SelectMaxMarkAptCardForQuery(pMission->m_lnTimeOut);
	if (pSelectedAptCard)
	{
		PrepareQuery(pMission, pSelectedAptCard);
	}
	else
	{
		CQueryMission * pMissionBackUp = new CQueryMission(*pMission);
		m_WaitingMissions.AddTail(pMissionBackUp);
	}
}

void CRoutingProtocolAptCard::SetStaticParameters(int nK, int nSeg, double fTrust, SIM_TIME lnAcTimeout)
{
	gm_fTrust = fTrust;
	CRoutingProcessAptCard::SetParameters(nK, nSeg, lnAcTimeout);
}

void CRoutingProtocolAptCard::SetLocalParameters(int nBswCopyCount)
{
	GetQueryProcess()->SetCopyCount(nBswCopyCount);
	GetReplyProcess()->SetCopyCount(nBswCopyCount);
}

COLORREF CRoutingProtocolAptCard::GetInportantLevel() const
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];

	if (GetReplyProcess()->GetDataMapSize() > 0)
	{
		return RGB(0, 0, 255);
	}
	else if (GetQueryProcess()->GetDataMapSize() > 0)
	{
		return RGB(255, 0, 0);
	}
	else
	{
		return RGB(0, 0, 0);
	}
}

int CRoutingProtocolAptCard::GetInfoList(CMsgShowInfo & allMessages) const
{
	int nDispense = 0, nReady = 0;
	GetAptCardProcess()->GetCardCount(nDispense, nReady);
	CMsgShowRow row;
	row.m_Item0.Format(_T("Dispense %d"), nDispense);
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Ready %d"), nReady);
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Query %d"), GetQueryProcess()->GetDataMapSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Reply %d"), GetReplyProcess()->GetDataMapSize());
	allMessages.m_Rows.AddTail(row);
	return 0;
}

void CRoutingProtocolAptCard::Turn(BOOL bOn)
{
	GetHelloProcess()->StartWork(TRUE);
}

int CRoutingProtocolAptCard::GetDebugNumber(int nParam)
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	return pAptCardProcess->GetCreatedCount();
}

CString CRoutingProtocolAptCard::GetDebugString() const
{
	return GetAptCardProcess()->GetAgencyListString();
}

void CRoutingProtocolAptCard::OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg)
{
	CList<CSentence *> sendingList;
	CPkgAptCardAck * pACAck = (CPkgAptCardAck *)pPkg;

	// APT CARD
	CPkgAptCardCards * pNewCards = GetAptCardProcess()->GetSendingList(pACAck->m_bAskForCards, pACAck->m_nHoldingReadyNumber, pACAck->m_pSender);
	sendingList.AddTail(pNewCards);

	// BSW REPLY
	GetReplyProcess()->OnEncounterUser(pPkg->m_pSender, sendingList, pACAck);

	CYell * pNewYell = new CYell();
	CRoutingProtocol * pTo = pNewYell->SetSentences(sendingList);
	TransmitMessage(pTo, pNewYell);
}

CPkgHello * CRoutingProtocolAptCard::GetHelloPackage(CRoutingProcessHello * pCallBy)
{
	CPkgAptCardHello * pNewHello = new CPkgAptCardHello();
	return pNewHello;
}

CPkgAck * CRoutingProtocolAptCard::GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo)
{
	BOOL bAskForCard = IsFriend(pTo);
	if (bAskForCard)
	{
		BOOL bLongTime = IsLongTimeNoSee(pTo);
		if (bLongTime == FALSE)
		{
			bAskForCard = FALSE;
		}
		else
		{
			m_lastTimeExchange.SetAt(pTo, GetSimTime());
		}
	}
	CPkgAptCardAck * pNewAck = new CPkgAptCardAck();
	pNewAck->m_bAskForCards = bAskForCard;
	pNewAck->m_pSpeakTo = pTo;
	int nDispense = 0, nReady = 0;
	GetAptCardProcess()->GetCardCount(nDispense, nReady);
	pNewAck->m_nHoldingReadyNumber = nReady;
	pNewAck->SetIds(m_PseduonymPairs);
	return pNewAck;
}

BOOL CRoutingProtocolAptCard::IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger)
{
	if (pCallBy == GetQueryProcess())
	{
		CPkgAptCardQuery * pQuery = (CPkgAptCardQuery *)pPkg;
		return pQuery->m_uReceiverId == pTheOther->GetHostId();
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgAptCardReply * pReply = (CPkgAptCardReply *)pPkg;
		if (pReply->m_bCheckId)
		{
			return pReply->m_uReceiverId == pTheOther->GetHostId();
		}
		else
		{
			CPkgAptCardAck * pAck = (CPkgAptCardAck *)pTriger;
			return IsReplyTmpDestination(pAck->m_pIDs, pAck->m_nIDNumber, pReply->m_Next);
		}
	}
	ASSERT(0);
	return TRUE;
}

BOOL CRoutingProtocolAptCard::IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		CPkgAptCardQuery * pQuery = (CPkgAptCardQuery *)pPkg;
		return pQuery->m_uReceiverId == GetHostId();
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgAptCardReply * pReply = (CPkgAptCardReply *)pPkg;
		if (pReply->m_bCheckId)
		{
			return pReply->m_uReceiverId == GetHostId();
		}
		else
		{

			POSITION posPseudonym = GetPseudonymPos(m_PseduonymPairs, pReply->m_Next);
			return posPseudonym != NULL;
		}
	}
	ASSERT(0);
	return TRUE;
}

CPkgBswData * CRoutingProtocolAptCard::CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		return new CPkgAptCardQuery(*(CPkgAptCardQuery*)pPkg);
	}
	if (pCallBy == GetReplyProcess())
	{
		return new CPkgAptCardReply(*(CPkgAptCardReply*)pPkg);
	}
	ASSERT(0);
	return NULL;
}

void CRoutingProtocolAptCard::OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	CString strLog;
	if (pCallBy == GetQueryProcess())
	{
		// LBS REPLY
		CPkgAptCardQuery * pQuery = (CPkgAptCardQuery *)pPkg;
		strLog.Format(_T("\nDelievery to %d"), GetHostId());
		WriteLog(strLog);
		CPkgAptCardReply * pNewReply = LbsPrepareReply(pQuery);
		GetReplyProcess()->InitNewPackage(pNewReply);
		pNewReply->m_pSpeakTo = pQuery->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);

		TransmitSingleSentence(pNewReply);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgAptCardReply * pReply = (CPkgAptCardReply *)pPkg;

		if (pReply->m_bCheckId)
		{
			// Is an agency
			CPkgAptCardReply * pNewReply = SwitchToNextAgency(pReply);

			strLog.Format(_T("\nAgency %d To %d"), GetHostId(), pNewReply->GetReceiverId());
			WriteLog(strLog);
			return;
		}
		else
		{
			// The original requirestor.
			strLog.Format(_T("\nOri req %d"), GetHostId());
			WriteLog(strLog);
			RemovePsedunym(pReply->m_Next);
		}
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolAptCard::OnGetNewCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg)
{
	PrepareAllWaitingQueries();
}

BOOL CRoutingProtocolAptCard::IsFriend(const CRoutingProtocol * pOther) const
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

BOOL CRoutingProtocolAptCard::IsLongTimeNoSee(const CRoutingProtocol * pOther) const
{
	SIM_TIME findValue;
	if (!m_lastTimeExchange.Lookup(pOther, findValue))
	{
		return TRUE;
	}
	else
	{
		SIM_TIME currentSimTime = GetSimTime();
		if (currentSimTime - findValue < 10000)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

CRoutingProcessAptCard * CRoutingProtocolAptCard::GetAptCardProcess() const
{
	return (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
}

CRoutingProcessBsw * CRoutingProtocolAptCard::GetQueryProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nQueryBswProcessId];
}

CRoutingProcessBsw * CRoutingProtocolAptCard::GetReplyProcess() const
{
	return (CRoutingProcessBsw*)m_Processes[m_nReplyBswProcessId];
}

CRoutingProcessHello * CRoutingProtocolAptCard::GetHelloProcess() const
{
	return (CRoutingProcessHello*)m_Processes[m_nHelloProcessId];
}

CPkgAptCardReply * CRoutingProtocolAptCard::LbsPrepareReply(const CPkgAptCardQuery * pQuery)
{
	CPkgAptCardReply * pNewReply = new CPkgAptCardReply();
	pNewReply->m_pTestSession = pQuery->DeepCopySession();
	pNewReply->m_Next.m_nGeneratorId = UID_INVALID;
	pNewReply->m_Next.m_nPseudonym = UID_INVALID;
	pNewReply->m_Next.m_nApt = pQuery->m_nAapt;
	pNewReply->m_uReceiverId = pQuery->m_nAid;
	pNewReply->m_lnTimeOut = pQuery->m_pTestSession->m_lnTimeOut;
	return pNewReply;
}

void CRoutingProtocolAptCard::PrepareAllWaitingQueries()
{
	POSITION pos = m_WaitingMissions.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CQueryMission * pMission = m_WaitingMissions.GetNext(pos);

		CAppointmentCard * pSelectedAptCard = GetAptCardProcess()->SelectMaxMarkAptCardForQuery(pMission->m_lnTimeOut);
		if (pSelectedAptCard == NULL)
		{
			return;
		}
		else
		{
			PrepareQuery(pMission, pSelectedAptCard);
			delete pMission;
			m_WaitingMissions.RemoveAt(posLast);
		}
	}
}

void CRoutingProtocolAptCard::PrepareQuery(const CQueryMission * pMission, CAppointmentCard * pSelectedAptCard)
{
	CTestRecordAptCard * pNewSessionRecord = new CTestRecordAptCard();
	gm_allSessions.SetAt(pMission->m_nMissionId, pNewSessionRecord);

	// Session in the query
	CTestSessionAptCard * pTestSession = new CTestSessionAptCard();
	pTestSession->InitSession(pMission->m_nMissionId);
	pTestSession->m_lnTimeOut = pMission->m_lnTimeOut;

	CPkgAptCardQuery * pNewQuery = new CPkgAptCardQuery();
	pNewQuery->m_pTestSession = pTestSession;
	pNewQuery->m_nAid = pSelectedAptCard->m_nCid;
	pNewQuery->m_nAapt = pSelectedAptCard->m_nCapt;

	GetQueryProcess()->InitNewPackage(pNewQuery, pMission->m_RecverId, pMission->m_lnTimeOut);
	GetQueryProcess()->InsertToDataMap(pNewQuery);

	CPseudonymPairRecord newPseudoPair;
	newPseudoPair.m_nGeneratorId = pSelectedAptCard->m_nAid;
	newPseudoPair.m_nApt = pSelectedAptCard->m_nAapt;
	newPseudoPair.m_lnTimeOut = pMission->m_lnTimeOut;
	CleanPseudonyms();
	m_PseduonymPairs.AddTail(newPseudoPair);
}

void CRoutingProtocolAptCard::CleanPseudonyms()
{
	SIM_TIME lnTime = GetSimTime();
	POSITION pos = m_PseduonymPairs.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		if (m_PseduonymPairs.GetNext(pos).m_lnTimeOut < lnTime)
		{
			m_PseduonymPairs.RemoveAt(posLast);
		}
	}
}

void CRoutingProtocolAptCard::RemovePsedunym(const CPseudonymPair & replyTarget)
{
	POSITION pos = GetPseudonymPos(m_PseduonymPairs, replyTarget);
	if (pos)
	{
		m_PseduonymPairs.RemoveAt(pos);
	}
}

BOOL CRoutingProtocolAptCard::IsReplyTmpDestination(CPseudonymPair * pPseudonymPairs, int nPseudonymNumber, const CPseudonymPair & replyTarget)
{
	for (int i = 0; i < nPseudonymNumber; ++i)
	{
		if (pPseudonymPairs[i] == replyTarget)
		{
			return TRUE;
		}
	}
	return FALSE;
}

POSITION CRoutingProtocolAptCard::GetPseudonymPos(const CList<CPseudonymPairRecord> & m_PseduonymPairs, const CPseudonymPair & replyTarget)
{
	POSITION pos = m_PseduonymPairs.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		if (m_PseduonymPairs.GetNext(pos) == replyTarget)
		{
			return posLast;
		}
	}
	return NULL;
}

void CRoutingProtocolAptCard::ResetAll()
{
}

CPkgAptCardReply * CRoutingProtocolAptCard::SwitchToNextAgency(const CPkgAptCardReply * pReply)
{
	CPkgAptCardReply * pNewReply = new CPkgAptCardReply(*pReply);
	CTestSessionAptCard * pSession = (CTestSessionAptCard*)pNewReply->m_pTestSession;

	CAptCardFromSameAgency * pFindAgencyRecord;
	USERID uOldId = pNewReply->m_Next.m_nGeneratorId;
	CAptCardAgencyRecord retRecord;
	if (GetAptCardProcess()->GetAndRemoveAgencyRecord(uOldId, pReply->m_Next.m_nApt, retRecord))
	{
		pNewReply->m_Next.m_nGeneratorId = GetHostId();
		pNewReply->m_Next.m_nApt = retRecord.n_nAAptNew;
		pNewReply->m_Next.m_nPseudonym = UID_INVALID;
		pNewReply->m_bCheckId = !retRecord.m_bTheLastRelay;
		GetReplyProcess()->InitNewPackage(pNewReply, retRecord.m_nExchangeTo, pSession->m_lnTimeOut);
		GetReplyProcess()->InsertToDataMap(pNewReply);
		return pNewReply;
	}
	else
	{
		ASSERT(0);
		return NULL;
	}
}

CMap<int, int, CTestRecordAptCard *, CTestRecordAptCard *> CRoutingProtocolAptCard::gm_allSessions;

double CRoutingProtocolAptCard::gm_fTrust = 0;
