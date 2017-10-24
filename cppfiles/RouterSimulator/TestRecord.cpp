#include "stdafx.h"
#include "TestRecord.h"


CTestRecord::CTestRecord()
	: m_nSessionId(0)
{
}


CTestRecord::CTestRecord(const CTestRecord & src)
{
	*this = src;
}

CTestRecord & CTestRecord::operator=(const CTestRecord & src)
{
	m_nSessionId = src.m_nSessionId;
	return *this;
}

CTestRecord::~CTestRecord()
{
}