#include "stdafx.h"
#include "StatisticSummary.h"
#include <fstream>
using namespace std;

CStatisticSummary::CStatisticSummary()
	: m_bEnd(TRUE)
	, m_pComments(NULL)
	, m_pWorkPath(NULL)
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
	if (m_pComments)
	{
		delete[] m_pComments;
	}
	if (m_pWorkPath)
	{
		delete[] m_pWorkPath;
	}
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
	int len = WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, NULL, 0, NULL, NULL);
	char *pProtocolName = new char[len + 10];
	WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, pProtocolName, len, NULL, NULL);

	ofstream fout;
	char filename[200] = {};
	for (int i = 0; i < m_nMaxSize; ++i)
	{
		if (m_pComments)
		{
			sprintf_s(filename, "\\ANALY_%s_E%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		else
		{
			sprintf_s(filename, "\\ANALY_%s_E%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		char filepath[200] = {};
		strcpy_s(filepath, m_pWorkPath);
		strcat_s(filepath, filename);
		fout.open(filepath, ios::app);
		fout << m_nRandomSeed << ";,";
		for (int j = 0; j < m_nTagIndex; ++j)
		{
			if (m_Tags[j].GetSize() > i)
			{
				fout << m_Tags[j][i] << ",";
			}
			else
			{
				ASSERT(0);
			}
		}
		fout << endl;
		fout.close();
	}
	delete[] pProtocolName;
}
