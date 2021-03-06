#include "stdafx.h"
#include "RoutingProtocolAptCard.h"
#include "RoutingProcessAptCard.h"
#include "TrustValue.h"
#include "MobileSocialNetworkHost.h"
#include "SentenceAptCard.h"
#include "HostEngine.h"

//#define JUST_HELLO


CRoutingProtocolAptCard::CRoutingProtocolAptCard()
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
	CRoutingProcessAptCard * pAptCardProcess = new CRoutingProcessAptCard();
	pAptCardProcess->SetProcessUser(this);
	m_nAptCardProcessId = AddProcess(pAptCardProcess);
	m_nSessionRecordEntrySize = REC_AC_ST_MAX;
	m_nProtocolRecordEntrySize = REC_AC_PTL_MAX;
	m_nSentAcCardPkgNumber = 0;
	m_nSentAcCardNumber = 0;
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

COLORREF CRoutingProtocolAptCard::GetImportantLevel() const
{
	if (GetAptCardProcess()->GetTrustListSize() == 0)
	{
		return RGB(255, 0, 0);
	}
	else
	{
		return RGB(0, 0, 0);
	}
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
	CRoutingProcessAptCard * pACProcess = GetAptCardProcess();
	CMsgShowRow row;
	row.m_Item0.Format(_T("Dispense %d"), pACProcess->GetDispenseListSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Ready %d"), pACProcess->GetReadyListSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Trust %d"), pACProcess->GetTrustListSize());
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

int CRoutingProtocolAptCard::GetCarryingPkgNumber(int nParam)
{
	switch (nParam)
	{
	case PROTOCOL_PKG_TYPE_SUPPLY:
	{
		return GetAptCardProcess()->GetAllAcListSize();
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

CString CRoutingProtocolAptCard::GetDebugString() const
{
	return GetAptCardProcess()->GetAgencyListString();
}

void CRoutingProtocolAptCard::OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev)
{
	GetHelloProcess()->OnSomeoneNearby(bAnyOneNearby, bDifferentFromPrev);
}

void CRoutingProtocolAptCard::OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg)
{
#ifdef JUST_HELLO
	return;
#endif
	CList<CSentence *> sendingList;
	CPkgAptCardAck * pACAck = (CPkgAptCardAck *)pPkg;

	// APT CARD
	CPkgAptCardCards * pNewCards = GetAptCardProcess()->GetSendingList(pACAck->m_bAskForCards, pACAck->m_nHoldingTrustNumber, pACAck->m_pSender);
	sendingList.AddTail(pNewCards);
	if (pNewCards->m_nCardNumber > 0)
	{
		m_nSentAcCardNumber += pNewCards->m_nCardNumber;
		SetProtocolRecordValue(REC_AC_PTL_SENT_CARD_NUMBER, m_nSentAcCardNumber);
		m_nSentAcCardPkgNumber++;
		SetProtocolRecordValue(REC_AC_PTL_SENT_CARDPKG_NUMBER, m_nSentAcCardPkgNumber);
	}

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
#ifdef JUST_HELLO
	BOOL bAskForCard = FALSE;
#else
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
#endif
	CPkgAptCardAck * pNewAck = new CPkgAptCardAck();
	pNewAck->m_bAskForCards = bAskForCard;
	pNewAck->m_pSpeakTo = pTo;
	pNewAck->m_nHoldingTrustNumber = GetAptCardProcess()->GetTrustListSize();
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

		SetSissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_AC_ST_REACH);
		SetSissionForwardNumber(pQuery->m_pTestSession->m_nSessionId, pQuery->m_pTestSession->m_nForwardNumber);
		if (!gm_bEnableLbsp)
		{
			return;
		}
		CPkgAptCardReply * pNewReply = LbsPrepareReply(pQuery);
		pNewReply->m_pTestSession->m_bInStatistic = FALSE;
		GetReplyProcess()->InitNewPackage(pNewReply);
		pNewReply->m_pSpeakTo = pQuery->m_pSender;
		GetReplyProcess()->MarkProcessIdToSentences(pNewReply);

		TransmitSingleSentence(pNewReply);

		SetSissionRecord(pQuery->m_pTestSession->m_nSessionId, REC_AC_ST_REP_LEAVE);
		return;
	}
	if (pCallBy == GetReplyProcess())
	{
		CPkgAptCardReply * pReply = (CPkgAptCardReply *)pPkg;

		if (pReply->m_bCheckId)
		{
			// Is an agency
			CPkgAptCardReply * pNewReply = SwitchToNextAgency(pReply);
			return;
		}
		else
		{
			// The original requirestor.
			if (SetSissionRecord(pReply->m_pTestSession->m_nSessionId, REC_AC_ST_REP_RETURN))
			{
				strLog.Format(_T("\nReturn to original %d"), GetHostId());
				WriteLog(strLog);
				RemovePsedunym(pReply->m_Next);
			}
		}
		return;
	}
	ASSERT(0);
}

void CRoutingProtocolAptCard::OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg)
{
	if (pCallBy == GetQueryProcess())
	{
		SetSissionRecord(pPkg->m_pTestSession->m_nSessionId, REC_AC_ST_FIRSTSEND);
	}
}

BOOL CRoutingProtocolAptCard::IsTrustful(CRoutingProcessAptCard * pCallBy, const CRoutingProtocol * pOther) const
{
	return IsFriend(pOther);
}

void CRoutingProtocolAptCard::OnGetNewCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg)
{
	SetProtocolRecordValue(REC_AC_PTL_HOLDING_TRUST_CARD_NUMBER, GetAptCardProcess()->GetTrustListSize());
	PrepareAllWaitingQueries();
	SendQueries(pPkg->m_pSender);
}

void CRoutingProtocolAptCard::OnGetNoneCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg)
{
	SendQueries(pPkg->m_pSender);
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
	SIM_TIME lnCT = GetSimTime();
	POSITION pos = m_WaitingMissions.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CQueryMission * pMission = m_WaitingMissions.GetNext(pos);
		if (pMission->m_lnTimeOut < lnCT)
		{
			delete pMission;
			m_WaitingMissions.RemoveAt(posLast);
			continue;
		}

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

	SetSissionRecord(pMission->m_nMissionId, REC_AC_ST_GENERATE);
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

void CRoutingProtocolAptCard::SendQueries(CRoutingProtocol * pTheOther)
{

	CList<CSentence *> sendingList;
	// BSW Query
	GetQueryProcess()->OnEncounterUser(pTheOther, sendingList, NULL);
	int nSendinglistLeng = sendingList.GetSize();
	CYell * pNewYell = new CYell();
	CRoutingProtocol * pTo = pNewYell->SetSentences(sendingList);
	TransmitMessage(pTo, pNewYell);
}

double CRoutingProtocolAptCard::gm_fTrust = 0;
