#include "stdafx.h"
#include "SentenceSlpd.h"


CPkgSlpd::CPkgSlpd()
{
	m_nSentenceType = SLPD_SENTENCE_TYPE_DATA;
}


CPkgSlpd::CPkgSlpd(const CPkgSlpd & src)
{
	*this = src;
}

CPkgSlpd & CPkgSlpd::operator=(const CPkgSlpd & src)
{
	CSentence::operator=(src);
	m_nPseudonym = src.m_nPseudonym;
	m_nRemainTimes = src.m_nRemainTimes;
	m_lnTimeout = src.m_lnTimeout;
	m_FriendList.RemoveAll();
	POSITION pos = src.m_FriendList.GetHeadPosition();
	while (pos)
	{
		m_FriendList.AddTail(src.m_FriendList.GetNext(pos));
	}
	m_RecverId = src.m_RecverId;
	return *this;
}

CPkgSlpd::~CPkgSlpd()
{
}

SLPD_USERID CPkgSlpd::GetOriginalRequester() const
{
	return m_FriendList.GetHead();
}

BOOL CPkgSlpd::IsAlreadyInFriendList(SLPD_USERID uId) const
{
	POSITION pos = m_FriendList.GetTailPosition();
	while (pos)
	{
		if (m_FriendList.GetPrev(pos) == uId)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CPkgSlpdAck::CPkgSlpdAck()
	: m_pIDs(NULL)
	, m_nIDNumber(0)
{

}

CPkgSlpdAck::CPkgSlpdAck(const CPkgSlpdAck & src)
	: m_pIDs(NULL)
	, m_nIDNumber(0)
{
	*this = src;
}

CPkgSlpdAck::~CPkgSlpdAck()
{

}

void CPkgSlpdAck::CleanIds()
{
	if (m_pIDs)
	{
		delete[] m_pIDs;
		m_pIDs = NULL;
		m_nIDNumber = 0;
	}
}

void CPkgSlpdAck::SetIds(const CList<CTimeOutPair<CSlpdUserAndPseudo>> & pseduonymPairs)
{
	CleanIds();
	m_nIDNumber = pseduonymPairs.GetSize();
	m_pIDs = new SLPD_USERID[m_nIDNumber];
	int i = 0;
	POSITION pos = pseduonymPairs.GetHeadPosition();
	while (pos)
	{
		m_pIDs[i++] = pseduonymPairs.GetNext(pos).m_Value.m_lnPseudonym;
	}
}

BOOL CPkgSlpdAck::IsInId(SLPD_USERID nID)
{
	for (int i = 0; i < m_nIDNumber; ++i)
	{
		if (m_pIDs[i] == nID)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CPkgSlpdAck & CPkgSlpdAck::operator=(const CPkgSlpdAck & src)
{
	CPkgAck::operator=(src);
	CleanIds();
	m_nIDNumber = src.m_nIDNumber;
	if (m_nIDNumber > 0)
	{
		m_pIDs = new SLPD_USERID[m_nIDNumber];
		for (int i = 0; i < m_nIDNumber; ++i)
		{
			m_pIDs[i] = src.m_pIDs[i];
		}
	}
	return *this;
}

CPkgSlpdReply::CPkgSlpdReply()
{

}

CPkgSlpdReply::CPkgSlpdReply(const CPkgSlpdReply & src)
{
	*this = src;
}

CPkgSlpdReply::~CPkgSlpdReply()
{

}

CPkgSlpdReply & CPkgSlpdReply::operator=(const CPkgSlpdReply & src)
{
	CPkgBswData::operator=(src);
	m_bIsPseudonym = src.m_bIsPseudonym;
	return *this;
}
