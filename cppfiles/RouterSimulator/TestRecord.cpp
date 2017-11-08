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
	: m_nSessionId(0)
	, m_nForwardTimes(0)
	, m_pMilestoneTime(NULL)
	, m_nMilestoneNumber(0)
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

CProtocolRecord::CProtocolRecord()
	: m_pData(NULL)
	, m_nDataLen(0)
{

}

CProtocolRecord::CProtocolRecord(int nDataNumber)
	: m_nDataLen(nDataNumber)
{
	if (nDataNumber > 0)
	{
		m_pData = new double[nDataNumber];
		memset(m_pData, 0, sizeof(double) * nDataNumber);
	}
	else
	{
		m_pData = NULL;
	}
}

CProtocolRecord::CProtocolRecord(const CProtocolRecord & src)
	: CProtocolRecord()
{
	*this = src;
}

CProtocolRecord::~CProtocolRecord()
{
	Reset();
}

void CProtocolRecord::Reset()
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
	m_nDataLen = 0;
}

CProtocolRecord & CProtocolRecord::operator=(const CProtocolRecord & src)
{
	Reset();
	m_nDataLen = src.m_nDataLen;
	if (m_nDataLen > 0)
	{
		m_pData = new double[m_nDataLen];
		memcpy_s(m_pData, m_nDataLen, src.m_pData, src.m_nDataLen);
	}
	m_pData = NULL;
	return *this;
}
