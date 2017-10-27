#pragma once
#include "SimulatorCommon.h"
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

	CArray<double> m_RealData;
	CArray<CString> m_StringData;

	CArray<CArray<double>> m_Tags;
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
	int m_nMaxSize;
	BOOL m_bEnd;
};

