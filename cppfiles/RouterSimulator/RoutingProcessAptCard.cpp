#include "stdafx.h"
#include "RoutingProcessAptCard.h"
#include "RoutingProtocol.h"
#include "EngineUser.h"
#include "HostEngine.h"
#include "RoadNet.h"
#include "TrustValue.h"
#include "MobileSocialNetworkHost.h"


CRoutingProcessAptCard::CRoutingProcessAptCard()
	: m_nNeededCardNumber(1000)
	, m_lnLastCreateAptCardTime(0)
	, m_lnLastCleanTime(0)
{
}


CRoutingProcessAptCard::~CRoutingProcessAptCard()
{
	ResetAll();
}

//  Owner gives the process a new query
void CRoutingProcessAptCard::GenerateMission(const CQueryMission * pMission)
{
	CAppointmentCard * pSelectedAptCard = SelectRandomAptCardForQuery();
	if (pSelectedAptCard == NULL)
	{
		CQueryMission * pMissionBackUp = new CQueryMission(*pMission);
		m_WaitingMissions.AddTail(pMissionBackUp);
		return;
	}
	PrepareQuery(pMission, pSelectedAptCard);
}

// Get a message from engine
void CRoutingProcessAptCard::OnReceivedMsg(const CYell * pYell)
{
	if (pYell->m_pSender == m_pProtocol)
	{
		return;
	}
	if (pYell->m_pRecver != NULL && pYell->m_pRecver != m_pProtocol)
	{
		return;
	}
	CList<CSentence*> answer;
	for (int i = 0; i < pYell->m_nSentenceCount; ++i)
	{
		int nSentenceType = pYell->m_ppSentences[i]->m_nSentenceType;
		switch (nSentenceType)
		{
		case SENTENCE_TYPE_HELLO:
		{
			OnReceivedHello(pYell->m_pSender, (CPkgAptCardHello*)pYell->m_ppSentences[i], answer);
			break;
		}
		case SENTENCE_TYPE_ACK:
		{
			OnReceivedHelloAck(pYell->m_pSender, (CPkgAptCardAck*)pYell->m_ppSentences[i], answer);
			break;
		}
		case AC_SENTENCE_TYPE_CARD:
		{
			OnReceivedCards(pYell->m_pSender, (CPkgAptCardCards*)pYell->m_ppSentences[i], answer);
			break;
		}
		case AC_SENTENCE_TYPE_QUERY:
		{
			OnReceivedQuery(pYell->m_pSender, (CPkgAptCardQuery*)pYell->m_ppSentences[i], answer);
			break;
		}
		case AC_SENTENCE_TYPE_REPLY:
		{
			OnReceivedReply(pYell->m_pSender, (CPkgAptCardReply*)pYell->m_ppSentences[i], answer);
			break;
		}
		default:
		{
			ASSERT(0);
		}
		}
	}
	if (answer.GetSize() > 0)
	{
		CYell * pNewYell = new CYell();
		CRoutingProtocol * pTo = pNewYell->SetSentences(answer);
		TransmitMessage(pTo, pNewYell);
	}
}

void CRoutingProcessAptCard::OnEngineTimer(int nCommandId)
{
	switch (nCommandId)
	{
	case AC_TIMER_CMD_HELLO:
	{
		SendHello();
		ContinueBinarySprayWaitProcess();
		break;
	}
	}
}

int CRoutingProcessAptCard::GetCarryingMessages(CMsgShowInfo & allMessages) const
{
	return 0;
}

void CRoutingProcessAptCard::TurnOn()
{
	StartBinarySprayWaitProcess();
}

void CRoutingProcessAptCard::SetParameters(int nK, int nSeg, int nCopyCount, double fTrust, SIM_TIME lnAcTimeout)
{
	m_nK = nK;
	m_nSeg = nSeg;
	m_nCopyCount = nCopyCount;
	m_fTrust = fTrust;
	m_lnAptCardsTimeout = lnAcTimeout;
}

// Move all apt cards which can be exchanged to the sending list.
void CRoutingProcessAptCard::OnCardsRequired(USERID nExchangeToId, const CPkgAptCardAck * pAck)
{
	CreateNewAptCards();
	PickDispensedCards(nExchangeToId);
	if (pAck->m_bAskForCards)
	{
		PickReadyCards(pAck->m_nHoldingReadyNumber);
	}
	PrepareToSend(nExchangeToId);
}

int CRoutingProcessAptCard::GetInfoList(CMsgShowInfo & allMessages) const
{
	CMsgShowRow row;
	row.m_Item0.Format(_T("Dispense %d"), m_DispensedCards.GetSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Ready %d"), m_ReadyCards.GetSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Query %d"), m_QueryList.GetSize());
	allMessages.m_Rows.AddTail(row);
	return 0;
}

COLORREF CRoutingProcessAptCard::GetInportantLevel() const
{
	if (m_ReplyList.GetSize() > 0)
	{
		return RGB(0, 0, 255);
	}
	else if (m_QueryList.GetSize() > 0)
	{
		return RGB(255, 0, 0);
	}
	else
	{
		return RGB(0, 0, 0);
	}
	int g = m_ReadyCards.GetSize() * 200 / m_nNeededCardNumber;
	if (g > 255)
	{
		return RGB(255, 255, 0);
	}
	else
	{
		return RGB(0, g, 0);
	}

}

int CRoutingProcessAptCard::GetReadyCount() const
{
	return m_ReadyCards.GetSize();
}

int CRoutingProcessAptCard::GetCreatedCount() const
{
	CAptCardFromSameAgency * pCreatList = NULL;
	m_AgencyList.Lookup(UID_INVALID, pCreatList);
	int nExistCardNumber = 0;
	if (pCreatList)
	{
		nExistCardNumber = pCreatList->m_Records.GetSize();
	}
	return nExistCardNumber;
}

CString CRoutingProcessAptCard::GetAgencyListString() const
{
	CString strOut;
	strOut.Format(_T("\nAL:%d"), m_pProtocol->GetHostId());
	POSITION pos = m_AgencyList.GetStartPosition();
	while (pos)
	{
		USERID rKey;
		CAptCardFromSameAgency * rValue;
		m_AgencyList.GetNextAssoc(pos, rKey, rValue);
		strOut += _T("\n") + rValue->GetString();
	}
	return strOut;
}

// Select to be exchanged dispensed cards.
void CRoutingProcessAptCard::PickDispensedCards(USERID nNextUser)
{
	POSITION pos, pospre;
	pos = m_DispensedCards.GetHeadPosition();
	while (pos)
	{
		pospre = pos;
		CAppointmentCard * pCurrentCard = m_DispensedCards.GetNext(pos);
		if (pCurrentCard->CheckDuplicatedUid(nNextUser))
		{
			continue;
		}

		--pCurrentCard->m_nDC;
		if (pCurrentCard->m_nDC == 0)
		{
			m_SendingList.AddTail(pCurrentCard);
			m_DispensedCards.RemoveAt(pospre);
		}
	}
}

// Create apt cards so that the total number reaches m_nNeededCardNumber
void CRoutingProcessAptCard::CreateNewAptCards()
{
	SweepExsitingAptNumber();

	int nExistCardNumber = GetCreatedCount();
	SIM_TIME lnDiffer = GetSimTime() - m_lnLastCreateAptCardTime;
	int nCreateCount = lnDiffer * m_nNeededCardNumber / m_lnAptCardsTimeout;
	if (nCreateCount > m_nNeededCardNumber)
	{
		nCreateCount = m_nNeededCardNumber;
	}

	CAptCardFromSameAgency * pCreatList = NULL;
	if (nCreateCount > 0)
	{
		m_lnLastCreateAptCardTime = GetSimTime();
		m_AgencyList.Lookup(UID_INVALID, pCreatList);
	}

	for (int i = 0; i < nCreateCount; ++i)
	{
		CAppointmentCard * pGeneratedCard = CreateSingleNewAptCard(pCreatList);
		m_SendingList.AddTail(pGeneratedCard);
	}
// 	for (int i = nExistCardNumber; i < m_nNeededCardNumber; ++i)
// 	{
// 		CAppointmentCard * pGeneratedCard = CreateSingleNewAptCard(pCreatList);
// 		m_SendingList.AddTail(pGeneratedCard);
// 	}
}

int CRoutingProcessAptCard::GetLastAcHopCardNumber() const
{
	int nRet = 0;
	POSITION pos = m_SendingList.GetHeadPosition();
	while (pos)
	{
		const CAppointmentCard * pCard = m_SendingList.GetNext(pos);
		if (IsTheLastCardRelay(pCard))
		{
			nRet++;
		}
	}
	return nRet;
}

// Pick ready apt cards.
void CRoutingProcessAptCard::PickReadyCards(int nTheOtherReadyNumber)
{
	int nEQLen = m_ReadyCards.GetSize();
	int nNumberOfK_1 = GetLastAcHopCardNumber();
	if (nNumberOfK_1 + nTheOtherReadyNumber >= nEQLen)
	{
		return;
	}
	int nSr = (nEQLen - nNumberOfK_1 - nTheOtherReadyNumber) / 2;

	char * pRandom = new char[nEQLen];
	BOOL bReverse = PickMNumberFromNArr(nSr, pRandom, nEQLen);
	char cPick = (bReverse ? 0 : 1);

	POSITION pos = m_ReadyCards.GetHeadPosition();
	int nListIndex = 0;
	while (pos)
	{
		POSITION posMove = pos;
		CAppointmentCard * pPicked = m_ReadyCards.GetNext(pos);
		if (pRandom[nListIndex++] == cPick)
		{
			m_SendingList.AddTail(pPicked);
			m_ReadyCards.RemoveAt(posMove);
		}
	}
}

BOOL CRoutingProcessAptCard::PickMNumberFromNArr(int nM, char * pArr, int nN)
{
	BOOL bReverse = FALSE;
	if (2 * nM > nN)
	{
		bReverse = TRUE;
		nM = nN - nM;
	}

	memset(pArr, 0, sizeof(char) * nN);
	int nIndex = rand() % nN;
	for (int i = 0; i < nM; ++i)
	{
		int nRand = rand() % (nN - i);
		int j = 0;
		while (TRUE)
		{
			if (pArr[nIndex] != 0)
			{
				nIndex = (nIndex + 1) % nN;
				continue;
			}
			else
			{
				if (j == nRand)
				{
					pArr[nIndex] = 1;
					nIndex = (nIndex + 1) % nN;
					break;
				}
				else
				{
					++j;
					nIndex = (nIndex + 1) % nN;
				}
			}
		}
	}
	return bReverse;
}

// The Last Step Before Send AC Out
void CRoutingProcessAptCard::PrepareToSend(USERID nNextUser)
{
	SIM_TIME lnCurrentSimTime = GetSimTime();

	POSITION pos = m_SendingList.GetHeadPosition();
	while (pos)
	{
		CAppointmentCard * pCurrentAC = m_SendingList.GetNext(pos);
		if (pCurrentAC->m_bIsReady)
		{
			continue;
		}

		CAptCardAgencyRecord newRecord;
		newRecord.InitByAptCard(pCurrentAC);
		newRecord.m_bTheLastRelay = IsTheLastCardRelay(pCurrentAC);
		newRecord.m_nExchangeTo = nNextUser;
		newRecord.n_nAAptNew = GetUniqueRandomNumber(lnCurrentSimTime + m_lnAptCardsTimeout);

		CAptCardFromSameAgency * pFindResult = NULL;
		if (!m_AgencyList.Lookup(pCurrentAC->m_nAid, pFindResult))
		{
			pFindResult = new CAptCardFromSameAgency();
			m_AgencyList[pCurrentAC->m_nAid] = pFindResult;
		}
		pFindResult->m_Records.AddTail(newRecord);
		pCurrentAC->m_EQ.AddTail(m_pProtocol->GetHostId());
		pCurrentAC->m_Mark.AddTail(GetMark());
		if (newRecord.m_bTheLastRelay)
		{
			pCurrentAC->m_bIsReady = TRUE;
		}
		pCurrentAC->m_nAid = m_pProtocol->GetHostId();
		pCurrentAC->m_nAapt = newRecord.n_nAAptNew;
	}
}

// Create One New Apt Card
CAppointmentCard * CRoutingProcessAptCard::CreateSingleNewAptCard(CAptCardFromSameAgency * pCreateList)
{
	SIM_TIME lnCurrentSimTime = GetSimTime();
	int nCApt = GetUniqueRandomNumber(lnCurrentSimTime + m_lnAptCardsTimeout);

	CAppointmentCard * newCard = new CAppointmentCard();
	newCard->m_nCid = m_pProtocol->GetHostId();
	newCard->m_nCapt = nCApt;
	newCard->m_nAid = UID_INVALID;
	newCard->m_nAapt = nCApt;
	newCard->m_lnTimeout = lnCurrentSimTime + m_lnAptCardsTimeout;
	//newCard->m_EQ.RemoveAll();
	newCard->m_nDC = 0;
	newCard->m_nPsd = UID_INVALID;
	return newCard;
}

// Create Capt or Aapt
int CRoutingProcessAptCard::GetUniqueRandomNumber(SIM_TIME lnTimeOut)
{
	SIM_TIME lnCurrentSimTime = GetSimTime();
	int nCApt = INVALID_APT_CARD_NUMBER;
	do
	{
		nCApt = rand();
		if (nCApt == INVALID_APT_CARD_NUMBER)
		{
			ASSERT(0);
			continue;
		}
		SIM_TIME findValue;
		if (m_ExistingCaptNumbers.Lookup(nCApt, findValue))
		{
			if (findValue >= lnCurrentSimTime)
			{
				nCApt = INVALID_APT_CARD_NUMBER;
			}
		}
	} while (nCApt == INVALID_APT_CARD_NUMBER);
	m_ExistingCaptNumbers[nCApt] = lnTimeOut;
	return nCApt;
}

// Remove useless apt numbers from m_ExistingCaptNumbers
void CRoutingProcessAptCard::SweepExsitingAptNumber()
{
	int nAgencyLength = m_AgencyList.GetSize();
	if (m_ExistingCaptNumbers.GetSize() < 3 * nAgencyLength)
	{
		return;
	}
	SIM_TIME lnCurrentSimTime = GetSimTime();
	CList<int> allToBeDeleteItems;
	POSITION pos = m_ExistingCaptNumbers.GetStartPosition();
	while (pos)
	{
		int rKey;
		SIM_TIME rValue;
		m_ExistingCaptNumbers.GetNextAssoc(pos, rKey, rValue);
		if (lnCurrentSimTime > rValue)
		{
			allToBeDeleteItems.AddTail(rKey);
		}
	}
	pos = allToBeDeleteItems.GetHeadPosition();
	while (pos)
	{
		int rKey = allToBeDeleteItems.GetNext(pos);
		m_ExistingCaptNumbers.RemoveKey(rKey);
	}
}

CAppointmentCard * CRoutingProcessAptCard::SelectRandomAptCardForQuery()
{
	return SelectMaxMarkAptCardForQuery();
	int nReadyCount = m_ReadyCards.GetSize();
	if (nReadyCount == 0)
	{
		return NULL;
	}
	POSITION pos = m_ReadyCards.GetHeadPosition();
	int nRand = rand() % nReadyCount;
	int nIndex = 0;
	while (pos)
	{
		if (nIndex == nRand)
		{
			CAppointmentCard * pRet = m_ReadyCards.GetAt(pos);
			m_ReadyCards.RemoveAt(pos);
			return pRet;
		}
		else
		{
			++nIndex;
			m_ReadyCards.GetNext(pos);
		}
	}
}

CAppointmentCard * CRoutingProcessAptCard::SelectMaxMarkAptCardForQuery()
{
	double nMaxMark = 0;
	POSITION posMax = NULL;
	CAppointmentCard * pRet = NULL;
	POSITION pos = m_ReadyCards.GetHeadPosition();
	while (pos)
	{
		CAppointmentCard * pCard = m_ReadyCards.GetAt(pos);
		double nTmpMark = TestAptCardMark(pCard);
		if (nTmpMark > nMaxMark)
		{
			nMaxMark = nTmpMark;
			posMax = pos;
			pRet = pCard;
		}
		m_ReadyCards.GetNext(pos);
	}
	if (pRet)
	{
		m_ReadyCards.RemoveAt(posMax);
	}
	return pRet;
}

BOOL CRoutingProcessAptCard::IsTheLastCardRelay(const CAppointmentCard * pCard) const
{
	if (pCard->m_bIsReady)
	{
		return FALSE;
	}
	if (m_nK - 1 == pCard->m_EQ.GetSize())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CRoutingProcessAptCard::StartBinarySprayWaitProcess()
{
	if (m_bWork == false)
	{
		m_bWork = true;
		SendHello();
		EngineRegisterTimer(AC_TIMER_CMD_HELLO, this, 1000);
	}
}

void CRoutingProcessAptCard::ContinueBinarySprayWaitProcess()
{
	if (m_bWork)
	{
		EngineRegisterTimer(AC_TIMER_CMD_HELLO, this, 1000);
	}
}

void CRoutingProcessAptCard::CleanTimeOutData()
{
	SIM_TIME lnTime = GetSimTime();
	POSITION pos = m_QueryList.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CPkgAptCardQuery * pMsg = m_QueryList.GetNext(pos);
		CTestSessionAptCard * pSession = (CTestSessionAptCard*)pMsg->m_pTestSession;
		if (pSession->m_lnTimeOut < lnTime)
		{
			delete pMsg;
			m_QueryList.RemoveAt(posLast);
		}
	}

	pos = m_ReplyList.GetHeadPosition();
	while (pos)
	{
		posLast = pos;
		CPkgAptCardReply * pMsg = m_ReplyList.GetNext(pos);
		CTestSessionAptCard * pSession = (CTestSessionAptCard*)pMsg->m_pTestSession;
		if (pSession->m_lnTimeOut < lnTime)
		{
			delete pMsg;
			m_ReplyList.RemoveAt(posLast);
		}
	}

	pos = m_DispensedCards.GetHeadPosition();
	while (pos)
	{
		posLast = pos;
		CAppointmentCard * pCard = m_DispensedCards.GetNext(pos);
		if (pCard->m_lnTimeout < lnTime)
		{
			delete pCard;
			m_DispensedCards.RemoveAt(posLast);
		}
	}

	pos = m_ReadyCards.GetHeadPosition();
	while (pos)
	{
		posLast = pos;
		CAppointmentCard * pCard = m_ReadyCards.GetNext(pos);
		if (pCard->m_lnTimeout < lnTime)
		{
			delete pCard;
			m_ReadyCards.RemoveAt(posLast);
		}
	}

	CleanTimeoutAgencyList();

	SIM_TIME lnEarliesTime = GetSimTime() - m_lnAptCardsTimeout * 2;
	while (m_EncounterUserList.GetHeadPosition())
	{
		if (m_EncounterUserList.GetHead() < lnEarliesTime)
		{
			m_EncounterUserList.RemoveHead();
		}
		else
		{
			break;
		}
	}

	pos = m_PseduonymPairs.GetHeadPosition();
	while (pos)
	{
		posLast = pos;
		if (m_PseduonymPairs.GetNext(pos).m_lnTimeOut < lnTime)
		{
			m_PseduonymPairs.RemoveAt(posLast);
		}
	}
}

void CRoutingProcessAptCard::SendHello()
{
	CPkgAptCardHello * pNewHello = new CPkgAptCardHello();
	CYell * pNewYell = new CYell();
	pNewYell->SetSentenceLength(1);
	pNewYell->m_ppSentences[0] = pNewHello;
	TransmitMessage(NULL, pNewYell);
}

// Hear a hello message.
void CRoutingProcessAptCard::OnReceivedHello(CRoutingProtocol * pSender, const CPkgAptCardHello * pHello, CList<CSentence*> & Answer)
{
	PrepareAck(pSender, Answer, TRUE);
	m_EncounterUserList.AddTail(GetSimTime());
}

// Hear an ACK message.
void CRoutingProcessAptCard::OnReceivedHelloAck(CRoutingProtocol * pSender, const CPkgAptCardAck * pAck, CList<CSentence*> & Answer)
{
	if (pAck->m_bCanAckBack)
	{
		if (m_pProtocol->GetHostId() == 92)
		{
			int k = 4;
		}
		if (m_lnLastCleanTime + 1000 < GetSimTime())
		{
			CleanTimeOutData();
			m_lnLastCleanTime = GetSimTime();
		}
		PrepareAck(pSender, Answer, FALSE);
	}
	OnCardsRequired(pSender->GetHostId(), pAck);

	CPkgAptCardCards * pNewCards = new CPkgAptCardCards();
	pNewCards->SetCards(m_SendingList);
	pNewCards->m_pProtocol = pSender;
	CleanSendingList();
	Answer.AddTail(pNewCards);
	ForwardReply(pSender, pAck, Answer);
}

void CRoutingProcessAptCard::OnReceivedCards(CRoutingProtocol * pSender, const CPkgAptCardCards * pCards, CList<CSentence*> & Answer)
{
	for (int i = 0; i < pCards->m_nCardNumber; ++i)
	{
		CAppointmentCard * pNewCard = new CAppointmentCard(pCards->m_pCards[i]);
		if (pNewCard->IsFinal())
		{
			m_ReadyCards.AddTail(pNewCard);
		}
		else
		{
			pNewCard->m_nDC = rand() % m_nSeg + 1;
			m_DispensedCards.AddTail(pNewCard);
		}
	}
	PrepareAllWaitingQueries();
	ForwardQuery(pSender, Answer);
}

void CRoutingProcessAptCard::OnReceivedQuery(CRoutingProtocol * pSender, const CPkgAptCardQuery * pQuery, CList<CSentence*> & Answer)
{
	CString strOut;
	if (pQuery->IsReceiver(m_pProtocol->GetHostId()))
	{
		SIM_TIME receivedTime = 0;
		if (m_DelieverMap.Lookup(pQuery->m_pTestSession->m_nSessionId, receivedTime))
		{
			return;
		}
		m_DelieverMap[pQuery->m_pTestSession->m_nSessionId] = GetSimTime();
		strOut.Format(_T("\nDelievery to %d"), m_pProtocol->GetHostId());
		WriteLog(strOut);
		CPkgAptCardReply * pNewReply = LbsPrepareReply(pQuery);
		pNewReply->m_pProtocol = pSender;
		Answer.AddTail(pNewReply);
	}
	else
	{
		CPkgAptCardReply * pMsgRecord = (CPkgAptCardReply*)CheckDuplicateInQueryList(pQuery->m_nBswId);
		if (pMsgRecord)
		{
			pMsgRecord->MergeMessage(*pQuery);
		}
		else
		{
			strOut.Format(_T("\nRelay by %d"), m_pProtocol->GetHostId());
			OutputDebugString(strOut);
			CPkgAptCardQuery * pNewRelayQuery = new CPkgAptCardQuery(*pQuery);
			m_QueryList.AddTail(pNewRelayQuery);
		}
	}
}

void CRoutingProcessAptCard::ForwardReply(CRoutingProtocol * pTo, const CPkgAptCardAck * pAck, CList<CSentence*> & Answer)
{
	POSITION pos = m_ReplyList.GetHeadPosition(), posLast;
	while (pos)
	{
		bool bSent = true;
		bool bDeleteMsg = false;
		posLast = pos;
		CPkgAptCardReply * pMsg = m_ReplyList.GetNext(pos);
		if (pMsg->m_bCheckId)
		{
			if (pMsg->IsReceiver(pTo->GetHostId()))
			{
				bDeleteMsg = true;
			}
		}
		else
		{
			if (IsReplyTmpDestination(pAck->m_pIDs, pAck->m_nIDNumber, pMsg->m_Next))
			{
				bDeleteMsg = true;
			}
		}

		if (!bDeleteMsg)
		{
			if (pMsg->OnlyOneCopyLeft())
			{
				bSent = false;
			}
			if (pMsg->IsKnownNode(pTo))
			{
				bSent = false;
			}
		}

		if (bSent)
		{
			CPkgAptCardReply * pNew = new CPkgAptCardReply(*pMsg);
			if (!bDeleteMsg)
			{
				pMsg->HalfCount(FALSE);
				pNew->HalfCount(TRUE);
			}
			pNew->m_bLastHop = bDeleteMsg;
			pNew->m_pProtocol = pTo;
			Answer.AddTail(pNew);
		}

		if (bDeleteMsg)
		{
			delete pMsg;
			m_ReplyList.RemoveAt(posLast);
		}
	}
}

void CRoutingProcessAptCard::OnReceivedReply(CRoutingProtocol * pSender, const CPkgAptCardReply * pReply, CList<CSentence*> & Answer)
{
	CString strOut;
	if (pReply->m_bLastHop)
	{
		if (pReply->m_bCheckId)
		{
			SIM_TIME lnRelyTime;
			if (m_RelayMap.Lookup(pReply->m_pTestSession->m_nSessionId, lnRelyTime))
			{
				return;
			}
			else
			{
				m_RelayMap[pReply->m_pTestSession->m_nSessionId] = GetSimTime();
			}
			strOut.Format(_T("\nAgency %d"), m_pProtocol->GetHostId());
			WriteLog(strOut);
			// Is an agency
			CPkgAptCardReply * pNewReply = GetRelayReply(pReply);
			CPkgAptCardReply * pSendReply = new CPkgAptCardReply(*pNewReply);
			pNewReply->HalfCount(FALSE);
			m_ReplyList.AddTail(pNewReply);
			strOut.Format(_T("\nAgency To %d"), pSendReply->GetReceiverId());
			WriteLog(strOut);

			pSendReply->HalfCount(TRUE);
			pSendReply->m_pProtocol = pSender;
			Answer.AddTail(pSendReply);
			return;
		}
		else
		{
			// The original requirestor.
			strOut.Format(_T("\nOri req %d"), m_pProtocol->GetHostId());
			OutputDebugString(strOut);
			POSITION posPseudonym = IsReplyTmpDestination(m_PseduonymPairs, pReply->m_Next);
			if (posPseudonym)
			{
				m_PseduonymPairs.RemoveAt(posPseudonym);
				// Delivered;
				return;
			}
		}
	}
	else
	{
		CPkgAptCardReply * pMsgRecord = (CPkgAptCardReply*)CheckDuplicateInReplyList(pReply->m_nBswId);
		if (pMsgRecord)
		{
			pMsgRecord->MergeMessage(*pReply);
		}
		else
		{
			CPkgAptCardReply * pNewRelayReply = new CPkgAptCardReply(*pReply);
			m_ReplyList.AddTail(pNewRelayReply);
		}
	}

}

void CRoutingProcessAptCard::PrepareAck(CRoutingProtocol * pTo, CList<CSentence*> & Answer, BOOL bBack)
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
			m_lastTimeExchange.SetAt(pTo, m_pProtocol->GetSimTime());
		}
	}
	CPkgAptCardAck * pNewAck = new CPkgAptCardAck();
	pNewAck->m_bAskForCards = bAskForCard;
	pNewAck->m_bCanAckBack = bBack;
	pNewAck->m_pProtocol = pTo;
	pNewAck->m_nHoldingReadyNumber = m_ReadyCards.GetSize();
	pNewAck->SetIds(m_PseduonymPairs);
	Answer.AddTail(pNewAck);
}

void CRoutingProcessAptCard::ForwardQuery(CRoutingProtocol * pTo, CList<CSentence*> & Answer)
{
	POSITION pos = m_QueryList.GetHeadPosition(), posLast;
	while (pos)
	{
		bool bSent = true;
		bool bDeleteMsg = false;
		posLast = pos;
		CPkgAptCardQuery * pMsg = m_QueryList.GetNext(pos);

		if (pMsg->IsReceiver(pTo->GetHostId()))
		{
			bDeleteMsg = true;
		}
		else
		{
			if (pMsg->OnlyOneCopyLeft())
			{
				bSent = false;
			}
			if (pMsg->IsKnownNode(pTo))
			{
				bSent = false;
			}
		}

		if (bSent)
		{
			CPkgAptCardQuery * pNew = new CPkgAptCardQuery(*pMsg);
			if (!bDeleteMsg)
			{
				pMsg->HalfCount(FALSE);
				pNew->HalfCount(TRUE);
			}
			pNew->m_pProtocol = pTo;
			Answer.AddTail(pNew);
		}

		if (bDeleteMsg)
		{
			delete pMsg;
			m_QueryList.RemoveAt(posLast);
		}
	}
}

void CRoutingProcessAptCard::PrepareAllWaitingQueries()
{
	POSITION pos = m_WaitingMissions.GetHeadPosition(), posLast;
	while (pos)
	{
		CAppointmentCard * pSelectedAptCard = SelectRandomAptCardForQuery();
		if (pSelectedAptCard == NULL)
		{
			return;
		}
		posLast = pos;
		PrepareQuery(m_WaitingMissions.GetNext(pos), pSelectedAptCard);
		delete m_WaitingMissions.GetAt(posLast);
		m_WaitingMissions.RemoveAt(posLast);
	}
}

void CRoutingProcessAptCard::PrepareQuery(const CQueryMission * pMission, CAppointmentCard * pSelectedAptCard)
{
	CString strLog = _T("\nUse Card !! ");
	strLog += pSelectedAptCard->GetString();
	WriteLog(strLog);
	CheckAptCards(pSelectedAptCard);
	CTestRecordAptCard * pNewSessionRecord = new CTestRecordAptCard();
	m_allSessions.SetAt(pMission->m_nMissionId, pNewSessionRecord);

	CPkgAptCardQuery * pNewQuery = new CPkgAptCardQuery();
	pNewQuery->InitParameters(m_nCopyCount, pMission->m_RecverId);
	CTestSessionAptCard * pTestSession = new CTestSessionAptCard();

	pTestSession->InitSession(pMission->m_nMissionId, pNewSessionRecord);
	pTestSession->m_nSender = m_pProtocol->GetHostId();
	ASSERT(pTestSession->m_nSender == pMission->m_SenderId);
	pTestSession->m_nLBS = pMission->m_RecverId;
	pTestSession->SetCardInfo(pSelectedAptCard->m_nCid, pSelectedAptCard->m_nCapt);
	pTestSession->m_lnTimeOut = pMission->m_lnTimeOut;

	pNewQuery->m_pTestSession = pTestSession;
	pNewQuery->m_nAid = pSelectedAptCard->m_nCid;
	pNewQuery->m_nAapt = pSelectedAptCard->m_nCapt;

	CQueryRecordEntry * pNewRecord = new CQueryRecordEntry();
	pNewRecord->m_pCard = new CAppointmentCard(*pSelectedAptCard);
	pNewRecord->m_nQueryId = pMission->m_nMissionId;
	m_MissionStatisticRecords.AddTail(pNewRecord);

	m_QueryList.AddTail(pNewQuery);
	CPseudonymPairRecord newPseudoPair;
	newPseudoPair.m_nGeneratorId = pSelectedAptCard->m_nAid;
	newPseudoPair.m_nApt = pSelectedAptCard->m_nAapt;
	newPseudoPair.m_lnTimeOut = pMission->m_lnTimeOut;
	m_PseduonymPairs.AddTail(newPseudoPair);
}

CPkgAptCardReply * CRoutingProcessAptCard::GetRelayReply(const CPkgAptCardReply * pReply)
{
	CPkgAptCardReply * pNewReply = new CPkgAptCardReply(*pReply);
	CTestSessionAptCard * pSession = (CTestSessionAptCard*)pNewReply->m_pTestSession;
	
	CAptCardFromSameAgency * pFindAgencyRecord;
	USERID uOldId = pNewReply->m_Next.m_nGeneratorId;
	if (m_AgencyList.Lookup(uOldId, pFindAgencyRecord))
	{
		POSITION pos = pFindAgencyRecord->m_Records.GetHeadPosition();
		while (pos)
		{
			POSITION posprev = pos;
			CAptCardAgencyRecord & tmpRecord = pFindAgencyRecord->m_Records.GetNext(pos);
			if (pReply->m_Next.m_nApt == tmpRecord.m_nAAptOld)
			{
				pNewReply->m_Next.m_nGeneratorId = m_pProtocol->GetHostId();
				pNewReply->m_Next.m_nApt = tmpRecord.n_nAAptNew;
				pNewReply->m_Next.m_nPseudonym = UID_INVALID;
				pNewReply->InitParameters(m_nCopyCount, tmpRecord.m_nExchangeTo);
				pNewReply->m_bCheckId = !tmpRecord.m_bTheLastRelay;

				pFindAgencyRecord->m_Records.RemoveAt(posprev);
				if (pFindAgencyRecord->m_Records.IsEmpty())
				{
					m_AgencyList.RemoveKey(uOldId);
				}
				return pNewReply;
			}
		}
		ASSERT(0);
		return NULL;
	}
	else
	{
		ASSERT(0);
		return NULL;
	}
}

CPkgAptCardReply * CRoutingProcessAptCard::LbsPrepareReply(const CPkgAptCardQuery * pQuery)
{
	CPkgAptCardReply * pNewReply = new CPkgAptCardReply();
	pNewReply->m_pTestSession = pQuery->DeepCopySession();
	pNewReply->m_Next.m_nGeneratorId = UID_INVALID;
	pNewReply->m_Next.m_nPseudonym = UID_INVALID;
	pNewReply->m_Next.m_nApt = pQuery->m_nAapt;
	pNewReply->InitParameters(m_nCopyCount, pQuery->m_nAid);
	return pNewReply;
}

CPkgBswData * CRoutingProcessAptCard::CheckDuplicateInReplyList(int nBswId)
{
	POSITION pos = m_ReplyList.GetHeadPosition();
	while (pos)
	{
		CPkgBswData * pMsgRecord = m_ReplyList.GetNext(pos);
		if (nBswId == pMsgRecord->m_nBswId)
		{
			return pMsgRecord;
		}
	}
	return NULL;
}

CPkgBswData * CRoutingProcessAptCard::CheckDuplicateInQueryList(int nBswId)
{
	POSITION pos = m_QueryList.GetHeadPosition();
	while (pos)
	{
		CPkgBswData * pMsgRecord = m_QueryList.GetNext(pos);
		if (nBswId == pMsgRecord->m_nBswId)
		{
			return pMsgRecord;
		}
	}
	return NULL;
}

BOOL CRoutingProcessAptCard::IsReplyTmpDestination(CPseudonymPair * pPseudonymPairs, int nPseudonymNumber, const CPseudonymPair & replyTarget)
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

POSITION CRoutingProcessAptCard::IsReplyTmpDestination(CList<CPseudonymPairRecord> & m_PseduonymPairs, const CPseudonymPair & replyTarget)
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

BOOL CRoutingProcessAptCard::IsFriend(CRoutingProtocol * pOther)
{
	CTrustValue fTrustValue;
	if (((CMobileSocialNetworkHost*)(m_pProtocol->GetHost()))->FindTrust(pOther->GetHostId(), fTrustValue))
	{
		if (fTrustValue > m_fTrust)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRoutingProcessAptCard::IsLongTimeNoSee(CRoutingProtocol * pOther)
{
	SIM_TIME findValue;
	if (!m_lastTimeExchange.Lookup(pOther, findValue))
	{
		return TRUE;
	}
	else
	{
		SIM_TIME currentSimTime = m_pProtocol->GetSimTime();
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

// Clean All AC in Sending List
void CRoutingProcessAptCard::CleanSendingList()
{
	POSITION pos = m_SendingList.GetHeadPosition();
	while (pos)
	{
		CAppointmentCard * pTmp = m_SendingList.GetNext(pos);
		delete pTmp;
	}
	m_SendingList.RemoveAll();
}

void CRoutingProcessAptCard::ResetAll()
{
	POSITION pos = m_MissionStatisticRecords.GetHeadPosition();
	while (pos)
	{
		delete m_MissionStatisticRecords.GetNext(pos);
	}
	m_MissionStatisticRecords.RemoveAll();

	pos = m_DispensedCards.GetHeadPosition();
	while (pos)
	{
		delete m_DispensedCards.GetNext(pos);
	}
	m_DispensedCards.RemoveAll();

	pos = m_ReadyCards.GetHeadPosition();
	while (pos)
	{
		delete m_ReadyCards.GetNext(pos);
	}
	m_ReadyCards.RemoveAll();
}

void CRoutingProcessAptCard::CleanTimeoutAgencyList()
{
	SIM_TIME lnTime = GetSimTime();
	POSITION pos = m_AgencyList.GetStartPosition();
	USERID rKey;
	CAptCardFromSameAgency * rValue;
	BOOL bDelSth = FALSE;
	while (pos)
	{
		m_AgencyList.GetNextAssoc(pos, rKey, rValue);
		POSITION rpos = rValue->m_Records.GetHeadPosition(), rposLast;
		while (rpos)
		{
			rposLast = rpos;
			CAptCardAgencyRecord rec = rValue->m_Records.GetNext(rpos);
			if (rec.m_lnTimeout < lnTime)
			{
				rValue->m_Records.RemoveAt(rposLast);
			}
		}
		if (rValue->m_Records.GetSize() == 0)
		{
			delete rValue;
			m_AgencyList.RemoveKey(rKey);
		}
	}

}

int CRoutingProcessAptCard::GetMark()
{
	if (m_EncounterUserList.IsEmpty())
	{
		return 1;
	}
	else
	{
		return m_EncounterUserList.GetSize();
	}
}

double CRoutingProcessAptCard::TestAptCardMark(CAppointmentCard * pCard)
{
	SIM_TIME lnCT = GetSimTime();
	SIM_TIME lnRemained = pCard->m_lnTimeout - lnCT;
	double nMark = lnRemained / 1000;
	if (nMark <= 0)
	{
		return nMark;
	}
	POSITION pos = pCard->m_Mark.GetHeadPosition();
	while (pos)
	{
		int nTmp = pCard->m_Mark.GetNext(pos);
		nMark *= nTmp;
	}
	return nMark;
}

void CRoutingProcessAptCard::CheckAptCards(const CAppointmentCard * pCard)
{
	POSITION pos = pCard->m_EQ.GetHeadPosition();
	while (pos)
	{
		USERID tmpid = pCard->m_EQ.GetNext(pos);
		CString strTmp;
		strTmp.Format(_T("LIST OF %d\n"), tmpid);
		strTmp += m_pProtocol->GetEngine()->GetRoadNet()->m_allHosts[tmpid]->m_pProtocol->GetDebugString();
		OutputDebugString(strTmp);
	}
}
