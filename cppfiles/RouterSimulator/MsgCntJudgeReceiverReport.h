#pragma once
#include "HostGui.h"
#include "SimulatorCommon.h"

class CReceiverReportItem
{
public:
	CReceiverReportItem();
	CReceiverReportItem(const CReceiverReportItem & src);
	CReceiverReportItem & operator = (const CReceiverReportItem & src);
	~CReceiverReportItem();
	BOOL m_bUnicast;
	CList<CHostGui> m_Hosts;
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
	CMap<CHost *, CHost *, CReceiverReportItem, CReceiverReportItem &> m_Items;
};

