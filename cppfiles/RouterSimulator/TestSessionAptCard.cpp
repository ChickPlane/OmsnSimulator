#include "stdafx.h"
#include "TestSessionAptCard.h"


CTestSessionAptCard::CTestSessionAptCard()
{
}


CTestSessionAptCard::CTestSessionAptCard(const CTestSessionAptCard & src)
{
	*this = src;
}

CTestSessionAptCard & CTestSessionAptCard::operator=(const CTestSessionAptCard & src)
{
	m_nSender = src.m_nSender;
	m_nLBS = src.m_nLBS;
	CTestSessionBsw::operator=(src);
	return *this;
}

CTestSessionAptCard::~CTestSessionAptCard()
{
}

void CTestSessionAptCard::SetCardInfo(USERID nCid, int nCapt)
{
	m_nCid = nCid;
	m_nCapt = nCapt;
}
