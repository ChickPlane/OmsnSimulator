#include "stdafx.h"
#include "PkgMhlpp.h"


CPkgMhlpp::CPkgMhlpp()
	: m_bInitialed(FALSE)
{
}


CPkgMhlpp::CPkgMhlpp(const CPkgMhlpp & src)
{
	*this = src;
}

CPkgMhlpp & CPkgMhlpp::operator=(const CPkgMhlpp & src)
{
	CPkgMultiHop::operator=(src);
	m_OriginalPosition = src.m_OriginalPosition;
	m_nPseudonym = src.m_nPseudonym;
	m_lnTimeout = src.m_lnTimeout;
	m_RecverId = src.m_RecverId;
	m_OriginalRequesterId = src.m_OriginalRequesterId;
	m_bInitialed = src.m_bInitialed;
	return *this;
}

CPkgMhlpp::~CPkgMhlpp()
{
}

CPkgMhlppAck::CPkgMhlppAck()
	: m_pIDs(NULL)
	, m_nIDNumber(0)
{

}

CPkgMhlppAck::CPkgMhlppAck(const CPkgMhlppAck & src)
	: m_pIDs(NULL)
	, m_nIDNumber(0)
{
	*this = src;
}

CPkgMhlppAck::~CPkgMhlppAck()
{

}

void CPkgMhlppAck::CleanIds()
{
	if (m_pIDs)
	{
		delete[] m_pIDs;
		m_pIDs = NULL;
	}
	m_nIDNumber = 0;
}

void CPkgMhlppAck::SetIds(const CList<CTimeOutPair<CMhlppUserAndPseudo>> & pseduonymPairs)
{
	CleanIds();
	m_nIDNumber = pseduonymPairs.GetSize();
	m_pIDs = new USERID[m_nIDNumber];
	int i = 0;
	POSITION pos = pseduonymPairs.GetHeadPosition();
	while (pos)
	{
		m_pIDs[i++] = pseduonymPairs.GetNext(pos).m_Value.m_lnPseudonym;
	}
}

BOOL CPkgMhlppAck::IsInId(USERID nID)
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

CPkgMhlppAck & CPkgMhlppAck::operator=(const CPkgMhlppAck & src)
{
	CPkgAck::operator=(src);
	CleanIds();
	m_nIDNumber = src.m_nIDNumber;
	if (m_nIDNumber > 0)
	{
		m_pIDs = new USERID[m_nIDNumber];
		memcpy(m_pIDs, src.m_pIDs, sizeof(USERID)* m_nIDNumber);
	}
	return *this;
}

CPkgMhlppReply::CPkgMhlppReply()
{

}

CPkgMhlppReply::CPkgMhlppReply(const CPkgMhlppReply & src)
{
	*this = src;
}

CPkgMhlppReply::~CPkgMhlppReply()
{
}

CPkgMhlppReply & CPkgMhlppReply::operator=(const CPkgMhlppReply & src)
{
	CPkgBswData::operator=(src);
	m_bIsPseudonym = src.m_bIsPseudonym;
	return *this;
}
