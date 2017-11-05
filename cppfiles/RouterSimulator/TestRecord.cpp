#include "stdafx.h"
#include "TestRecord.h"


CTestRecord::CTestRecord()
	: m_nSessionId(0)
	, m_nForwardTimes(0)
	, m_pMilestoneTime(NULL)
	, m_nMilestoneNumber(0)
{
}


CTestRecord::CTestRecord(const CTestRecord & src)
{
	*this = src;
}

CTestRecord::CTestRecord(int nMilestoneNumber)
	: m_nSessionId(0)
	, m_nForwardTimes(0)
{
	m_nMilestoneNumber = nMilestoneNumber;
	if (m_nMilestoneNumber == 0)
	{
		m_pMilestoneTime = NULL;
	}
	else
	{
		m_pMilestoneTime = new SIM_TIME[m_nMilestoneNumber];
		memset(m_pMilestoneTime, -1, sizeof(SIM_TIME)*m_nMilestoneNumber);
	}
}

CTestRecord & CTestRecord::operator=(const CTestRecord & src)
{
	m_nSessionId = src.m_nSessionId;
	m_nForwardTimes = src.m_nForwardTimes;
	Reset();
	m_nMilestoneNumber = src.m_nMilestoneNumber;
	if (m_nMilestoneNumber > 0)
	{
		m_pMilestoneTime = new SIM_TIME[m_nMilestoneNumber];
		memcpy(m_pMilestoneTime, src.m_pMilestoneTime, sizeof(SIM_TIME)*m_nMilestoneNumber);
	}
	return *this;
}

CTestRecord::~CTestRecord()
{
	Reset();
}

void CTestRecord::Reset()
{
	if (m_pMilestoneTime)
	{
		delete[] m_pMilestoneTime;
		m_pMilestoneTime = NULL;
	}
	m_nMilestoneNumber = 0;
}
