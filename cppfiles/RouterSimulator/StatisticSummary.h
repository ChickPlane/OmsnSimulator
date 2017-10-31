#pragma once
#include "SimulatorCommon.h"

enum
{
	SS_TOTLE_YELL = 0,
	SS_ENGINE_MAX
};

class CStatisticSummaryTag
{
public:
	CArray<double> m_ProtocolRecords;
	CArray<double> m_EngineRecords;
	CStatisticSummaryTag();
	CStatisticSummaryTag(const CStatisticSummaryTag & src) { *this = src; }
	CStatisticSummaryTag & operator=(const CStatisticSummaryTag & src);
};

class CStatisticSummary
{
public:
	CStatisticSummary();
	CStatisticSummary(const CStatisticSummary & src);
	CStatisticSummary & operator = (const CStatisticSummary & src);
	virtual ~CStatisticSummary();

	void StartTest(SIM_TIME lnStartTime, SIM_TIME lnTestEndTime, SIM_TIME Interval);
	void AddTag(SIM_TIME lnStartTime);
	void OutputResult();
	BOOL IsWorking() const { return !m_bEnd; }

	CStatisticSummaryTag m_RecentData;

	CArray<CStatisticSummaryTag> m_Tags;
	CString m_ProtocolName;
	int m_nRandomSeed;
	double m_fCommuRadius;
	double m_fTrust;
	char * m_pComments;
	char * m_pWorkPath;

private:
	SIM_TIME m_lnTestStartTime;
	SIM_TIME m_Interval;
	SIM_TIME m_lnTestEndTime;
	int m_nTagIndex;
	int m_nMaxProtocolSize;
	int m_nMaxEngineSize;
	BOOL m_bEnd;
};

