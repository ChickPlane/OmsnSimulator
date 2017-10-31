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
	m_nMaxProtocolSize = 0;
	m_nMaxEngineSize = 0;
	int nExpectSize = (lnTestEndTime - lnStartTime) / Interval;
	m_RecentData.m_EngineRecords[SS_TOTLE_YELL] = 0;
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
	int nSize = m_RecentData.m_ProtocolRecords.GetSize();
	if (nSize > m_nMaxProtocolSize)
	{
		m_nMaxProtocolSize = nSize;
	}
	nSize = m_RecentData.m_EngineRecords.GetSize();
	if (nSize > m_nMaxEngineSize)
	{
		m_nMaxEngineSize = nSize;
	}
	m_Tags[m_nTagIndex] = m_RecentData;
	++m_nTagIndex;
}

void CStatisticSummary::OutputResult()
{
	int len = WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, NULL, 0, NULL, NULL);
	char *pProtocolName = new char[len + 10];
	WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, pProtocolName, len, NULL, NULL);

	ofstream fout;
	char filename[200] = {};
	for (int i = 0; i < m_nMaxProtocolSize; ++i)
	{
		if (m_pComments)
		{
			sprintf_s(filename, "\\ANALY_%s_P%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		else
		{
			sprintf_s(filename, "\\ANALY_%s_P%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		char filepath[200] = {};
		strcpy_s(filepath, m_pWorkPath);
		strcat_s(filepath, filename);
		fout.open(filepath, ios::app);
		fout << m_nRandomSeed << ";,";
		for (int j = 0; j < m_nTagIndex; ++j)
		{
			if (m_Tags[j].m_ProtocolRecords.GetSize() > i)
			{
				fout << m_Tags[j].m_ProtocolRecords[i] << ",";
			}
			else
			{
				ASSERT(0);
			}
		}
		fout << endl;
		fout.close();
	}

	for (int i = 0; i < m_nMaxEngineSize; ++i)
	{
		if (m_pComments)
		{
			sprintf_s(filename, "\\ANALY_%s_ENG%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		else
		{
			sprintf_s(filename, "\\ANALY_%s_ENG%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		char filepath[200] = {};
		strcpy_s(filepath, m_pWorkPath);
		strcat_s(filepath, filename);
		fout.open(filepath, ios::app);
		fout << m_nRandomSeed << ";,";
		for (int j = 0; j < m_nTagIndex; ++j)
		{
			if (m_Tags[j].m_EngineRecords.GetSize() > i)
			{
				fout << m_Tags[j].m_EngineRecords[i] << ",";
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

CStatisticSummaryTag & CStatisticSummaryTag::operator=(const CStatisticSummaryTag & src)
{
	int nPr = src.m_ProtocolRecords.GetSize();
	m_ProtocolRecords.SetSize(nPr);
	for (int i = 0; i < nPr; ++i)
	{
		m_ProtocolRecords[i] = src.m_ProtocolRecords[i];
	}
	int nEr = src.m_EngineRecords.GetSize();
	m_EngineRecords.SetSize(nEr);
	for (int i = 0; i < nEr; ++i)
	{
		m_EngineRecords[i] = src.m_EngineRecords[i];
	}
	return *this;
}

CStatisticSummaryTag::CStatisticSummaryTag()
{
	m_EngineRecords.SetSize(SS_ENGINE_MAX);
}
