#include "stdafx.h"
#include "StatisticSummary.h"
#include <fstream>
using namespace std;


CStatisticProtocolSummaryTag::CStatisticProtocolSummaryTag()
{

}



CStatisticProtocolSummaryTag & CStatisticProtocolSummaryTag::operator=(const CStatisticProtocolSummaryTag & src)
{
	int nPr = src.m_ProtocolRecords.GetSize();
	m_ProtocolRecords.SetSize(nPr);
	for (int i = 0; i < nPr; ++i)
	{
		m_ProtocolRecords[i] = src.m_ProtocolRecords[i];
	}

	return *this;
}

CStatisticSummary::CStatisticSummary()
	: m_bEnd(TRUE)
	, m_pComments(NULL)
	, m_pWorkPath(NULL)
	, m_Interval(60*1000)
	, m_nProtocolTagIndex(0)
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

void CStatisticSummary::StartTest(SIM_TIME lnStartTime, SIM_TIME lnTestEndTime)
{
	m_bEnd = FALSE;
	m_lnTestStartTime = lnStartTime;
	m_lnTestEndTime = lnTestEndTime;
	m_nSessionTagIndex = 0;
	m_nMaxSessionSize = 0;
	m_nMaxProtocolSize = 0;
	m_nMaxEngineSize = 0;
	int nExpectSize = (lnTestEndTime - lnStartTime) / m_Interval;
	m_RecentSessionTag.m_EngineRecords[SS_TOTLE_YELL] = 0;
	m_SessionTags.SetSize(nExpectSize);
}

BOOL CStatisticSummary::IsTimeForSessionTag(SIM_TIME lnStartTime) const
{
	if (m_bEnd)
	{
		return FALSE;
	}
	if (lnStartTime < m_lnTestStartTime + (m_nSessionTagIndex + 1) * m_Interval)
	{
		return FALSE;
	}
	return TRUE;
}

void CStatisticSummary::AddSessionTag(SIM_TIME lnStartTime)
{
	if (!IsTimeForSessionTag(lnStartTime))
	{
		return;
	}
	if (m_SessionTags.GetSize() > m_nSessionTagIndex)
	{
		int nSize = m_RecentSessionTag.m_SessionRecords.GetSize();
		if (nSize > m_nMaxSessionSize)
		{
			m_nMaxSessionSize = nSize;
		}
		nSize = m_RecentSessionTag.m_EngineRecords.GetSize();
		if (nSize > m_nMaxEngineSize)
		{
			m_nMaxEngineSize = nSize;
		}
		m_SessionTags[m_nSessionTagIndex] = m_RecentSessionTag;
		++m_nSessionTagIndex;
	}
	if (lnStartTime > m_lnTestEndTime || m_nSessionTagIndex == m_SessionTags.GetSize())
	{
		OutputResult();
		m_bEnd = TRUE;
	}
}

void CStatisticSummary::AddProtocolTag(SIM_TIME lnStartTime)
{
	if (lnStartTime < (m_ProtocolTags.GetSize() + 1) * m_Interval)
	{
		return;
	}
	int nSize = m_RecentProtocolTag.m_ProtocolRecords.GetSize();
	if (nSize > m_nMaxProtocolSize)
	{
		m_nMaxProtocolSize = nSize;
	}
	m_ProtocolTags.AddTail(m_RecentProtocolTag);
}

void CStatisticSummary::OutputResult()
{
	int len = WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, NULL, 0, NULL, NULL);
	char *pProtocolName = new char[len + 10];
	WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, pProtocolName, len, NULL, NULL);

	ofstream fout;
	char filename[200] = {};
	for (int i = 0; i < m_nMaxSessionSize; ++i)
	{
		if (m_pComments)
		{
			sprintf_s(filename, "\\%s_S%d_%s.csv", pProtocolName, i, m_pComments);
		}
		else
		{
			ASSERT(0);
			sprintf_s(filename, "\\%s_S%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		char filepath[200] = {};
		strcpy_s(filepath, m_pWorkPath);
		strcat_s(filepath, filename);
		fout.open(filepath, ios::app);
		fout << m_nRandomSeed << ";,";
		for (int j = 0; j < m_nSessionTagIndex; ++j)
		{
			if (m_SessionTags[j].m_SessionRecords.GetSize() > i)
			{
				fout << m_SessionTags[j].m_SessionRecords[i] << ",";
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
			sprintf_s(filename, "\\%s_Eng%d_%s.csv", pProtocolName, i, m_pComments);
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
		for (int j = 0; j < m_nSessionTagIndex; ++j)
		{
			if (m_SessionTags[j].m_EngineRecords.GetSize() > i)
			{
				fout << m_SessionTags[j].m_EngineRecords[i] << ",";
			}
			else
			{
				ASSERT(0);
			}
		}
		fout << endl;
		fout.close();
	}

	for (int i = 0; i < m_nMaxProtocolSize; ++i)
	{
		if (m_pComments)
		{
			sprintf_s(filename, "\\%s_P%d_%s.csv", pProtocolName, i, m_pComments);
		}
		else
		{
			ASSERT(0);
			sprintf_s(filename, "\\%s_P%02d_C%d_%s.csv", pProtocolName, i, (int)m_fCommuRadius, m_pComments);
		}
		char filepath[200] = {};
		strcpy_s(filepath, m_pWorkPath);
		strcat_s(filepath, filename);
		fout.open(filepath, ios::app);
		fout << m_nRandomSeed << ";,";
		POSITION posPtl = m_ProtocolTags.GetHeadPosition();
		while (posPtl)
		{
			CStatisticProtocolSummaryTag & tmpPtlTag = m_ProtocolTags.GetNext(posPtl);
			if (tmpPtlTag.m_ProtocolRecords.GetSize() > i)
			{
				fout << tmpPtlTag.m_ProtocolRecords[i] << ",";
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

BOOL CStatisticSummary::IsCompleted(SIM_TIME lnTime) const
{
	return lnTime > m_lnTestEndTime;
}

CStatisticSessionSummaryTag & CStatisticSessionSummaryTag::operator=(const CStatisticSessionSummaryTag & src)
{
	int nSr = src.m_SessionRecords.GetSize();
	m_SessionRecords.SetSize(nSr);
	for (int i = 0; i < nSr; ++i)
	{
		m_SessionRecords[i] = src.m_SessionRecords[i];
	}

	int nEr = src.m_EngineRecords.GetSize();
	m_EngineRecords.SetSize(nEr);
	for (int i = 0; i < nEr; ++i)
	{
		m_EngineRecords[i] = src.m_EngineRecords[i];
	}
	return *this;
}

CStatisticSessionSummaryTag::CStatisticSessionSummaryTag()
{
	m_EngineRecords.SetSize(SS_ENGINE_MAX);
}
