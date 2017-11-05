#include "stdafx.h"
#include "TestSession.h"


CTestSession::CTestSession()
	: m_nSessionId(0)
	, m_lnTimeOut(0)
	, m_nForwardNumber(0)
	, m_bInStatistic(TRUE)
{
}


CTestSession::CTestSession(const CTestSession & src)
{
	*this = src;
}

CTestSession & CTestSession::operator=(const CTestSession & src)
{
	m_bInStatistic = src.m_bInStatistic;
	m_nSessionId = src.m_nSessionId;
	m_lnTimeOut = src.m_lnTimeOut;
	m_nForwardNumber = src.m_nForwardNumber;
	return *this;
}

CTestSession::~CTestSession()
{
}

void CTestSession::InitSession(int nSessionId)
{
	m_nSessionId = nSessionId;
}
