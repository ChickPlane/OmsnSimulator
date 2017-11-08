#pragma once
#include "SimulatorCommon.h"

enum
{
	SS_TOTLE_YELL = 0,
	SS_AVE_SUPPLY_BUFFER,
	SS_AVE_QUERY_BUFFER,
	SS_AVE_REPLY_BUFFER,
	SS_ENGINE_MAX
};

class CStatisticProtocolSummaryTag
{
public:
	CArray<double> m_ProtocolRecords;
	CStatisticProtocolSummaryTag();
	CStatisticProtocolSummaryTag(const CStatisticProtocolSummaryTag & src) { *this = src; }
	CStatisticProtocolSummaryTag & operator=(const CStatisticProtocolSummaryTag & src);
};

class CStatisticSessionSummaryTag
{
public:
	CArray<double> m_SessionRecords;
	CArray<double> m_EngineRecords;
	CStatisticSessionSummaryTag();
	CStatisticSessionSummaryTag(const CStatisticSessionSummaryTag & src) { *this = src; }
	CStatisticSessionSummaryTag & operator=(const CStatisticSessionSummaryTag & src);
};

class CStatisticSummary
{
public:
	CStatisticSummary();
	CStatisticSummary(const CStatisticSummary & src);
	CStatisticSummary & operator = (const CStatisticSummary & src);
	virtual ~CStatisticSummary();

	void StartTest(SIM_TIME lnStartTime, SIM_TIME lnTestEndTime);
	BOOL IsTimeForSessionTag(SIM_TIME lnStartTime) const;
	void AddSessionTag(SIM_TIME lnStartTime);
	void AddProtocolTag(SIM_TIME lnStartTime);
	void OutputResult();
	BOOL IsSessionRecordWorking() const { return !m_bEnd; }
	BOOL IsCompleted(SIM_TIME lnTime) const;

	CStatisticSessionSummaryTag m_RecentSessionTag;
	CStatisticProtocolSummaryTag m_RecentProtocolTag;

	CArray<CStatisticSessionSummaryTag> m_SessionTags;
	CList<CStatisticProtocolSummaryTag> m_ProtocolTags;
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
	int m_nSessionTagIndex;
	int m_nProtocolTagIndex;
	int m_nMaxSessionSize;
	int m_nMaxProtocolSize;
	int m_nMaxEngineSize;
	BOOL m_bEnd;
};

