#include "stdafx.h"
#include "SentenceAptCard.h"


CPkgAptCardHello::CPkgAptCardHello()
{

}

CPkgAptCardHello::CPkgAptCardHello(const CPkgAptCardHello & src)
{
	*this = src;
}

CPkgAptCardHello::~CPkgAptCardHello()
{

}

CPkgAptCardAck::CPkgAptCardAck()
	: m_bAskForCards(FALSE)
	, m_pIDs(NULL)
	, m_nIDNumber(0)
{

}

CPkgAptCardAck::CPkgAptCardAck(const CPkgAptCardAck & src)
{
	*this = src;
}

CPkgAptCardAck::~CPkgAptCardAck()
{

}

CPkgAptCardAck & CPkgAptCardAck::operator=(const CPkgAptCardAck & src)
{
	CPkgAck::operator=(src);
	m_bAskForCards = src.m_bAskForCards;
	m_nHoldingTrustNumber = src.m_nHoldingTrustNumber;
	CleanIds();
	m_nIDNumber = src.m_nIDNumber;
	if (m_nIDNumber > 0)
	{
		m_pIDs = new CPseudonymPair[m_nIDNumber];
		for (int i = 0; i < m_nIDNumber; ++i)
		{
			m_pIDs[i] = src.m_pIDs[i];
		}
	}
	return *this;
}

void CPkgAptCardAck::CleanIds()
{
	if (m_pIDs)
	{
		delete[] m_pIDs;
		m_pIDs = NULL;
		m_nIDNumber = 0;
	}
}

void CPkgAptCardAck::SetIds(const CList<CPseudonymPairRecord> & pseduonymPairs)
{
	CleanIds();
	m_nIDNumber = pseduonymPairs.GetSize();
	m_pIDs = new CPseudonymPair[m_nIDNumber];
	int i = 0;
	POSITION pos = pseduonymPairs.GetHeadPosition();
	while (pos)
	{
		m_pIDs[i++] = pseduonymPairs.GetNext(pos);
	}
}

CPkgAptCardQuery::CPkgAptCardQuery()
	: m_nAid(UID_INVALID)
	, m_nAapt(0)
{
}

CPkgAptCardQuery::CPkgAptCardQuery(const CPkgAptCardQuery & src)
{
	*this = src;
}

CPkgAptCardQuery & CPkgAptCardQuery::operator=(const CPkgAptCardQuery & src)
{
	CPkgBswData::operator=(src);
	m_nAid = src.m_nAid;
	m_nAapt = src.m_nAapt;
	return *this;
}

CPkgAptCardQuery::~CPkgAptCardQuery()
{
}

CTestSessionAptCard * CPkgAptCardQuery::DeepCopySession() const
{
	if (m_pTestSession)
	{
		return new CTestSessionAptCard(*(CTestSessionAptCard*)m_pTestSession);
	}
	else
	{
		return NULL;
	}
}

CPkgAptCardReply::CPkgAptCardReply()
	: m_bCheckId(TRUE)
{
}

CPkgAptCardReply::CPkgAptCardReply(const CPkgAptCardReply & src)
{
	*this = src;
}

CPkgAptCardReply::~CPkgAptCardReply()
{
}

CTestSessionAptCard * CPkgAptCardReply::DeepCopySession() const
{
	if (m_pTestSession)
	{
		return new CTestSessionAptCard(*(CTestSessionAptCard*)m_pTestSession);
	}
	else
	{
		return NULL;
	}
}

CPkgAptCardReply & CPkgAptCardReply::operator=(const CPkgAptCardReply & src)
{
	CPkgBswData::operator=(src);
	m_bCheckId = src.m_bCheckId;
	m_Next = src.m_Next;
	return *this;
}

CPkgAptCardCards::CPkgAptCardCards()
	: m_pCards(NULL)
	, m_nCardNumber(0)
{
	m_nSentenceType = AC_SENTENCE_TYPE_CARD;
}

CPkgAptCardCards::CPkgAptCardCards(const CPkgAptCardCards & src)
{
	*this = src;
}

CPkgAptCardCards::~CPkgAptCardCards()
{
	CleanCards();
}

void CPkgAptCardCards::CleanCards()
{
	if (m_pCards)
	{
		delete[] m_pCards;
		m_pCards = NULL;
		m_nCardNumber = 0;
	}
}

void CPkgAptCardCards::SetCardCount(int nCardNumber)
{
	CleanCards();
	m_nCardNumber = nCardNumber;
	m_pCards = new CAppointmentCard[nCardNumber];
}

void CPkgAptCardCards::SetCards(CList<CAppointmentCard *> & SendingList)
{
	int nCardCount = SendingList.GetSize();
	SetCardCount(nCardCount);
	int nIndex = 0;
	POSITION pos = SendingList.GetHeadPosition();
	while (pos)
	{
		m_pCards[nIndex++] = *SendingList.GetNext(pos);
	}
}

CPkgAptCardCards & CPkgAptCardCards::operator=(const CPkgAptCardCards & src)
{
	CSentence::operator=(src);
	SetCardCount(src.m_nCardNumber);
	if (m_nCardNumber > 0)
	{
		for (int i = 0; i < m_nCardNumber; ++i)
		{
			m_pCards[i] = src.m_pCards[i];
		}
	}
	return *this;
}
