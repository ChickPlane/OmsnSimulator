#include "stdafx.h"
#include "TestSession.h"


CTestSession::CTestSession()
	: m_pRecord(NULL)
	, m_nSessionId(0)
{
}


CTestSession::CTestSession(const CTestSession & src)
{
	*this = src;
}

CTestSession & CTestSession::operator=(const CTestSession & src)
{
	m_nSessionId = src.m_nSessionId;
	m_pRecord = src.m_pRecord;
	return *this;
}

CTestSession::~CTestSession()
{
}

void CTestSession::InitSession(int nSessionId, CTestRecord * pRecord)
{
	m_nSessionId = nSessionId;
	m_pRecord = pRecord;
}
