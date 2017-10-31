#include "stdafx.h"
#include "TestRecord.h"


CTestRecord::CTestRecord()
	: m_nSessionId(0)
	, m_nForwardTimes(0)
{
}


CTestRecord::CTestRecord(const CTestRecord & src)
{
	*this = src;
}

CTestRecord & CTestRecord::operator=(const CTestRecord & src)
{
	m_nSessionId = src.m_nSessionId;
	m_nForwardTimes = src.m_nForwardTimes;
	return *this;
}

CTestRecord::~CTestRecord()
{
}
