#include "stdafx.h"
#include "TestSession.h"


CTestSession::CTestSession()
	: m_nSessionId(0)
	, m_lnTimeOut(0)
{
}


CTestSession::CTestSession(const CTestSession & src)
{
	*this = src;
}

CTestSession & CTestSession::operator=(const CTestSession & src)
{
	m_nSessionId = src.m_nSessionId;
	m_lnTimeOut = src.m_lnTimeOut;
	return *this;
}

CTestSession::~CTestSession()
{
}

void CTestSession::InitSession(int nSessionId)
{
	m_nSessionId = nSessionId;
}
