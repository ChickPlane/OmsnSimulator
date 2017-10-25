#include "stdafx.h"
#include "StatisticSummary.h"
#include <fstream>
using namespace std;

CStatisticSummary::CStatisticSummary()
	: m_bEnd(TRUE)
{
}


CStatisticSummary::CStatisticSummary(const CStatisticSummary & src)
{
	*this = src;
}

CStatisticSummary & CStatisticSummary::operator=(const CStatisticSummary & src)
{
	return *this;
}

CStatisticSummary::~CStatisticSummary()
{
}

void CStatisticSummary::StartTest(SIM_TIME lnStartTime, SIM_TIME lnTestEndTime, SIM_TIME Interval)
{
	m_bEnd = FALSE;
	m_lnTestStartTime = lnStartTime;
	m_lnTestEndTime = lnTestEndTime;
	m_Interval = Interval;
	m_nTagIndex = 0;
	m_nMaxSize = 0;
	int nExpectSize = (lnTestEndTime - lnStartTime) / Interval;
	m_Tags.SetSize(nExpectSize);
}

void CStatisticSummary::AddTag(SIM_TIME lnStartTime)
{
	if (m_bEnd)
	{
		return;
	}
	if (lnStartTime > m_lnTestEndTime)
	{
		OutputResult();
		m_bEnd = TRUE;
	}
	if (lnStartTime < m_lnTestStartTime + (m_nTagIndex + 1) * m_Interval)
	{
		return;
	}
	int nSize = m_RealData.GetSize();
	if (nSize > m_nMaxSize)
	{
		m_nMaxSize = nSize;
	}
	m_Tags[m_nTagIndex].SetSize(nSize);
	for (int i = 0; i < nSize; ++i)
	{
		m_Tags[m_nTagIndex][i] = m_RealData[i];
	}
	++m_nTagIndex;
}

void CStatisticSummary::OutputResult()
{
	ofstream fout;
	char filename[200] = {};
	for (int i = 0; i < m_nMaxSize; ++i)
	{
		sprintf_s(filename, "ANALY_%s_E%02d_C%d_T%d.csv", m_ProtocolName.GetBuffer(0), i, (int)m_fCommuRadius, (int)(m_fTrust * 100));
		fout.open(filename, ios::app);
		fout << m_nRandomSeed << ";\t";
		for (int j = 0; j < m_nTagIndex; ++j)
		{
			if (m_Tags[j].GetSize() > i)
			{
				fout << m_Tags[j][i] << "\t";
			}
			else
			{
				ASSERT(0);
			}
		}
		fout << endl;
		fout.close();
	}
}
