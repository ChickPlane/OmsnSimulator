#include "stdafx.h"
#include "RoutingProcessAptCard.h"
#include "RoutingProtocol.h"
#include "EngineUser.h"
#include "HostEngine.h"
#include "RoadNet.h"
#include "TrustValue.h"
#include "MobileSocialNetworkHost.h"
#include "CommonFunctions.h"
#include "RoutingProtocolAptCard.h"


CRoutingProcessAptCard::CRoutingProcessAptCard()
	: m_nNeededCardNumber(50)
	, m_lnLastCreateAptCardTime(0)
	, m_lnLastCleanTime(0)
{
}


CRoutingProcessAptCard::~CRoutingProcessAptCard()
{
	ResetAll();
}

void CRoutingProcessAptCard::SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol)
{
	CRoutingProcess::SetBasicParameters(nProcessID, pProtocol);
}

CPkgAptCardCards * CRoutingProcessAptCard::GetSendingList(BOOL bNeedReady, int nHoldingCount, CRoutingProtocol * pTo)
{
	CleanTimeOutData();
	CList<CAppointmentCard*> cardList;
	CreateNewAptCards(cardList);
	PickDispensedCards(pTo->GetHostId(), cardList);

	if (bNeedReady)
	{
		PickReadyAndTrustCards(nHoldingCount, cardList);
	}
	PrepareToSend(pTo->GetHostId(), cardList);


	CPkgAptCardCards * pNewCards = new CPkgAptCardCards();
	pNewCards->SetCards(cardList);
	if (pNewCards->HasDuplicated())
	{
		ASSERT(0);
	}
	pNewCards->m_pSpeakTo = pTo;
	pNewCards->m_pSender = m_pProtocol;
	CleanSendingList(cardList);

	MarkProcessIdToSentences(pNewCards);

	return pNewCards;
}

void CRoutingProcessAptCard::OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList)
{
	int nSentenceType = pPkg->m_nSentenceType;
	switch (nSentenceType)
	{
	case AC_SENTENCE_TYPE_CARD:
	{
		OnReceivedCards((CPkgAptCardCards*)pPkg);
		break;
	}
	default:
	{
		ASSERT(0);
	}
	}
}

void CRoutingProcessAptCard::OnEngineTimer(int nCommandId)
{
	ASSERT(0);
}

int CRoutingProcessAptCard::GetCarryingMessages(CMsgShowInfo & allMessages) const
{
	return 0;
}

void CRoutingProcessAptCard::SetParameters(int nK, int nSeg, SIM_TIME lnAcTimeout)
{
	gm_nK = nK;
	gm_nSeg = nSeg;
	gm_lnAptCardsTimeout = lnAcTimeout;
}

BOOL CRoutingProcessAptCard::GetAndRemoveAgencyRecord(USERID uOldId, int nOldApt, CAptCardAgencyRecord & retRecord)
{
	CAptCardFromSameAgency * pFindAgencyRecord = NULL;
	if (m_AgencyList.Lookup(uOldId, pFindAgencyRecord))
	{
		POSITION pos = pFindAgencyRecord->m_Records.GetHeadPosition();
		while (pos)
		{
			POSITION posprev = pos;
			CAptCardAgencyRecord & tmpRecord = pFindAgencyRecord->m_Records.GetNext(pos);
			if (nOldApt == tmpRecord.m_nAAptOld)
			{
				retRecord = tmpRecord;
				pFindAgencyRecord->m_Records.RemoveAt(posprev);
				if (pFindAgencyRecord->m_Records.IsEmpty())
				{
					m_AgencyList.RemoveKey(uOldId);
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CRoutingProcessAptCard::GetInfoList(CMsgShowInfo & allMessages) const
{
	CMsgShowRow row;
	row.m_Item0.Format(_T("Dispense %d"), m_DispensedCards.GetSize());
	allMessages.m_Rows.AddTail(row);
	row.m_Item0.Format(_T("Ready %d"), m_ReadyCards.GetSize());
	allMessages.m_Rows.AddTail(row);
	return 0;
}

COLORREF CRoutingProcessAptCard::GetInportantLevel() const
{
	return RGB(50, 50, 50);
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
void CRoutingProcessAptCard::PickDispensedCards(USERID nNextUser, CList<CAppointmentCard*> & SendingList)
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
			SendingList.AddTail(pCurrentCard);
			m_DispensedCards.RemoveAt(pospre);
		}
	}
}

// Create apt cards so that the total number reaches m_nNeededCardNumber
void CRoutingProcessAptCard::CreateNewAptCards(CList<CAppointmentCard*> & SendingList)
{
	SweepExsitingAptNumber();

	int nExistCardNumber = GetCreatedCount();
	SIM_TIME lnDiffer = GetSimTime() - m_lnLastCreateAptCardTime;
	int nCreateCount = lnDiffer * m_nNeededCardNumber / gm_lnAptCardsTimeout;
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

		SendingList.AddTail(pGeneratedCard);
	}

	if (pCreatList)
	{
		m_pProtocol->SetProtocolRecordValue(REC_AC_PTL_EXIST_CREATED_CARD_NUMBER, pCreatList->m_Records.GetSize());
	}
}

int CRoutingProcessAptCard::GetLastAcHopCardNumber(const CList<CAppointmentCard*> & SendingList)
{
	int nRet = 0;
	POSITION pos = SendingList.GetHeadPosition();
	while (pos)
	{
		const CAppointmentCard * pCard = SendingList.GetNext(pos);
		if (IsTheLastCardRelay(pCard))
		{
			nRet++;
		}
	}
	return nRet;
}

// Pick ready apt cards.
void CRoutingProcessAptCard::PickReadyAndTrustCards(int nTheOtherReadyNumber, CList<CAppointmentCard*> & SendingList)
{
	int nEQLen = GetReadyCount();
	if (nTheOtherReadyNumber >= nEQLen)
	{
		return;
	}
	int nSr = (nEQLen - nTheOtherReadyNumber) / 2;
	char * pRandom = new char[nEQLen];
	BOOL bReverse = FALSE;
	char cPick = 0;
	int nReadySize = GetReadyListSize();
	int nTrustSize = GetTrustListSize();
	if (nSr < nReadySize)
	{
		CCommonFunctions::PickMFromNDisorder(nSr, pRandom, nReadySize);
		POSITION pos = m_ReadyCards.GetHeadPosition();
		int nListIndex = 0;
		while (pos)
		{
			POSITION posMove = pos;
			CAppointmentCard * pPicked = m_ReadyCards.GetNext(pos);
			if (pRandom[nListIndex++] != 0)
			{
				SendingList.AddTail(pPicked);
				m_ReadyCards.RemoveAt(posMove);
			}
		}

	}
	else
	{
		POSITION pos = m_ReadyCards.GetHeadPosition();
		while (pos)
		{
			CAppointmentCard * pPicked = m_ReadyCards.GetNext(pos);
			SendingList.AddTail(pPicked);
		}
		m_ReadyCards.RemoveAll();

		if (nSr > nReadySize)
		{
			CCommonFunctions::PickMFromNDisorder(nSr - nReadySize, pRandom, nTrustSize);
			POSITION pos = m_TrustCards.GetHeadPosition();
			int nListIndex = 0;
			while (pos)
			{
				POSITION posMove = pos;
				CAppointmentCard * pPicked = m_TrustCards.GetNext(pos);
				if (pRandom[nListIndex++] != 0)
				{
					SendingList.AddTail(pPicked);
					m_TrustCards.RemoveAt(posMove);
				}
			}

		}
	}
	delete[] pRandom;
}

// The Last Step Before Send AC Out
void CRoutingProcessAptCard::PrepareToSend(USERID nNextUser, CList<CAppointmentCard*> & SendingList)
{
	SIM_TIME lnCurrentSimTime = GetSimTime();

	POSITION pos = SendingList.GetHeadPosition();
	while (pos)
	{
		CAppointmentCard * pCurrentAC = SendingList.GetNext(pos);
		if (pCurrentAC->m_bIsReady)
		{
			continue;
		}

		CAptCardAgencyRecord newRecord;
		newRecord.InitByAptCard(pCurrentAC);
		newRecord.m_bTheLastRelay = IsTheLastCardRelay(pCurrentAC);
		if (!newRecord.m_bTheLastRelay)
		{
			newRecord.m_nExchangeTo = nNextUser;
		}
		else
		{
			newRecord.m_nExchangeTo = -1;
		}
		newRecord.n_nAAptNew = GetUniqueRandomNumber(lnCurrentSimTime + gm_lnAptCardsTimeout);

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
	int nCApt = GetUniqueRandomNumber(lnCurrentSimTime + gm_lnAptCardsTimeout);

	CAppointmentCard * newCard = new CAppointmentCard();
	newCard->m_nCid = m_pProtocol->GetHostId();
	newCard->m_nCapt = nCApt;
	newCard->m_nAid = UID_INVALID;
	newCard->m_nAapt = nCApt;
	newCard->m_lnTimeout = lnCurrentSimTime + gm_lnAptCardsTimeout;
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

CAppointmentCard * CRoutingProcessAptCard::SelectMaxMarkAptCardForQuery(SIM_TIME lnTimeout)
{
	double nMaxMark = INT_MAX;
	POSITION posMax = NULL;
	CAppointmentCard * pRet = NULL;
	POSITION pos = m_TrustCards.GetHeadPosition();
	while (pos)
	{
		CAppointmentCard * pCard = m_TrustCards.GetAt(pos);
		double nTmpMark = TestAptCardMark(pCard, lnTimeout);
		if (nTmpMark < nMaxMark)
		{
			nMaxMark = nTmpMark;
			posMax = pos;
			pRet = pCard;
		}
		m_TrustCards.GetNext(pos);
	}
	if (pRet)
	{
		m_TrustCards.RemoveAt(posMax);
	}
	return pRet;
}

BOOL CRoutingProcessAptCard::IsTheLastCardRelay(const CAppointmentCard * pCard)
{
	if (pCard->m_bIsReady)
	{
		return FALSE;
	}
	if (gm_nK - 1 == pCard->m_EQ.GetSize())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CRoutingProcessAptCard::CleanTimeOutData()
{
	SIM_TIME lnTime = GetSimTime();
	POSITION pos = m_DispensedCards.GetHeadPosition(), posLast;
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

	pos = m_TrustCards.GetHeadPosition();
	while (pos)
	{
		posLast = pos;
		CAppointmentCard * pCard = m_TrustCards.GetNext(pos);
		if (pCard->m_lnTimeout < lnTime)
		{
			delete pCard;
			m_TrustCards.RemoveAt(posLast);
		}
	}

	CleanTimeoutAgencyList();

	SIM_TIME lnEarliesTime = GetSimTime() - gm_lnAptCardsTimeout * 2;
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
}

void CRoutingProcessAptCard::SendHello()
{
	CPkgAptCardHello * pNewHello = new CPkgAptCardHello();
	CYell * pNewYell = new CYell();
	pNewYell->SetSentenceLength(1);
	pNewYell->m_ppSentences[0] = pNewHello;
	TransmitMessage(NULL, pNewYell);
}

void CRoutingProcessAptCard::OnReceivedCards(const CPkgAptCardCards * pCards)
{
	for (int i = 0; i < pCards->m_nCardNumber; ++i)
	{
		CAppointmentCard * pNewCard = new CAppointmentCard(pCards->m_pCards[i]);
		if (pNewCard->IsFinal())
		{
			if (m_pUser->IsTrustful(this, pCards->m_pSender))
			{
				pNewCard->m_ReceiveTime = GetSimTime();
				m_TrustCards.AddTail(pNewCard);
			}
			else
			{
				pNewCard->m_ReceiveTime = GetSimTime();
				m_ReadyCards.AddTail(pNewCard);
			}
		}
		else
		{
			pNewCard->m_nDC = rand() % gm_nSeg + 1;
			m_DispensedCards.AddTail(pNewCard);
		}
	}
	if (pCards->m_nCardNumber > 0)
	{
		m_pUser->OnGetNewCards(this, pCards);
	}
	else
	{
		m_pUser->OnGetNoneCards(this, pCards);
	}
}

int CRoutingProcessAptCard::GetAllAcListSize() const
{
	return GetTrustListSize() + GetReadyListSize() + GetDispenseListSize();
}

// Clean All AC in Sending List
void CRoutingProcessAptCard::CleanSendingList(CList<CAppointmentCard*> & SendingList)
{
	POSITION pos = SendingList.GetHeadPosition();
	while (pos)
	{
		CAppointmentCard * pTmp = SendingList.GetNext(pos);
		delete pTmp;
	}
	SendingList.RemoveAll();
}

void CRoutingProcessAptCard::ResetAll()
{
	POSITION pos = m_DispensedCards.GetHeadPosition();
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

	pos = m_TrustCards.GetHeadPosition();
	while (pos)
	{
		delete m_TrustCards.GetNext(pos);
	}
	m_TrustCards.RemoveAll();
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

double CRoutingProcessAptCard::TestAptCardMark(CAppointmentCard * pCard, SIM_TIME lnTimeout)
{
	SIM_TIME lnCT = GetSimTime();
	if (lnTimeout <= lnCT)
	{
		ASSERT(0);
		return 0;
	}
	if (pCard->m_lnTimeout <= lnTimeout)
	{
		return 0;
	}

	return pCard->m_ReceiveTime - pCard->m_lnTimeout + gm_lnAptCardsTimeout;

	SIM_TIME lnRemained = lnTimeout - lnCT;
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

int CRoutingProcessAptCard::GetReadyCount() const
{
	return m_ReadyCards.GetSize() + m_TrustCards.GetSize();
}

BOOL CRoutingProcessAptCard::CheckDuplicated()
{
	CMap<int, int, CIdAndApt, CIdAndApt> mapAllAc;

	POSITION ppp = m_TrustCards.GetHeadPosition();
	while (ppp)
	{
		CAppointmentCard * pttt = m_TrustCards.GetNext(ppp);
		CIdAndApt tmpIA;
		int nnn = 0;
		if (mapAllAc.Lookup(pttt->m_nCapt, tmpIA))
		{
			ASSERT(0);
		}
		else
		{
			tmpIA.nId = pttt->m_nCid;
			tmpIA.nApt = pttt->m_nCapt;
			tmpIA.nGroup = 1;
			mapAllAc[pttt->m_nCapt] = tmpIA;
		}
	}
	ppp = m_ReadyCards.GetHeadPosition();
	while (ppp)
	{
		CAppointmentCard * pttt = m_ReadyCards.GetNext(ppp);
		CIdAndApt tmpIA;
		int nnn = 0;
		if (mapAllAc.Lookup(pttt->m_nCapt, tmpIA))
		{
			ASSERT(0);
		}
		else
		{
			tmpIA.nId = pttt->m_nCid;
			tmpIA.nApt = pttt->m_nCapt;
			tmpIA.nGroup = 2;
			mapAllAc[pttt->m_nCapt] = tmpIA;
		}
	}

	ppp = m_DispensedCards.GetHeadPosition();
	while (ppp)
	{
		CAppointmentCard * pttt = m_DispensedCards.GetNext(ppp);
		CIdAndApt tmpIA;
		int nnn = 0;
		if (mapAllAc.Lookup(pttt->m_nCapt, tmpIA))
		{
			ASSERT(0);
		}
		else
		{
			tmpIA.nId = pttt->m_nCid;
			tmpIA.nApt = pttt->m_nCapt;
			tmpIA.nGroup = 3;
			mapAllAc[pttt->m_nCapt] = tmpIA;
		}
	}
	return FALSE;
}

int CRoutingProcessAptCard::gm_nK;

int CRoutingProcessAptCard::gm_nSeg;

SIM_TIME CRoutingProcessAptCard::gm_lnAptCardsTimeout;
