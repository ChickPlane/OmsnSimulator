#pragma once
#include "HostGui.h"
#include "SimulatorCommon.h"

class CJudgeTmpRouteEntry
{
public:
	CJudgeTmpRouteEntry();
	CJudgeTmpRouteEntry(const CJudgeTmpRouteEntry & src);
	CJudgeTmpRouteEntry & operator = (const CJudgeTmpRouteEntry & src);
	CHostGui m_HopFrom;
	CList<CHostGui *> m_HopDestinations;
};

class CReceiverReportItem
{
public:
	CReceiverReportItem();
	CReceiverReportItem(const CReceiverReportItem & src);
	CReceiverReportItem & operator = (const CReceiverReportItem & src);
	~CReceiverReportItem();
	BOOL m_bUnicast;
	CList<CHostGui> m_Hosts;
	CHost * m_pCenterHost;
};

class CMsgCntJudgeReceiverReport
{
public:
	CMsgCntJudgeReceiverReport();
	CMsgCntJudgeReceiverReport(const CMsgCntJudgeReceiverReport & src);
	CMsgCntJudgeReceiverReport & operator = (const CMsgCntJudgeReceiverReport & src);
	~CMsgCntJudgeReceiverReport();

	BOOL m_bFullReport;
	SIM_TIME m_lnTime;
	CArray<CReceiverReportItem> m_ArrItems;
};

