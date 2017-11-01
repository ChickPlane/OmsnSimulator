#include "stdafx.h"
#include "MsgCntJudgeReceiverReport.h"


CReceiverReportItem::CReceiverReportItem()
	: m_bUnicast(FALSE)
{

}


CReceiverReportItem::CReceiverReportItem(const CReceiverReportItem & src)
{
	*this = src;
}

CReceiverReportItem::~CReceiverReportItem()
{

}

CReceiverReportItem & CReceiverReportItem::operator=(const CReceiverReportItem & src)
{
	m_bUnicast = src.m_bUnicast;
	m_pCenterHost = src.m_pCenterHost;
	POSITION pos = src.m_Hosts.GetHeadPosition();
	while (pos)
	{
		m_Hosts.AddTail(src.m_Hosts.GetNext(pos));
	}
	return *this;
}

CMsgCntJudgeReceiverReport::CMsgCntJudgeReceiverReport()
	: m_bFullReport(FALSE)
{
}


CMsgCntJudgeReceiverReport::CMsgCntJudgeReceiverReport(const CMsgCntJudgeReceiverReport & src)
{
	*this = src;
}

CMsgCntJudgeReceiverReport & CMsgCntJudgeReceiverReport::operator=(const CMsgCntJudgeReceiverReport & src)
{
	ASSERT(0);
	m_bFullReport = src.m_bFullReport;
	m_lnTime = src.m_lnTime;

	int nSize = src.m_ArrItems.GetSize();
	m_ArrItems.SetSize(nSize);
	for (int i = 0; i < nSize; ++i)
	{
		m_ArrItems[i] = src.m_ArrItems[i];
	}
	return *this;
}

CMsgCntJudgeReceiverReport::~CMsgCntJudgeReceiverReport()
{
}

CJudgeTmpRouteEntry::CJudgeTmpRouteEntry()
{

}

CJudgeTmpRouteEntry::CJudgeTmpRouteEntry(const CJudgeTmpRouteEntry & src)
{

}

CJudgeTmpRouteEntry & CJudgeTmpRouteEntry::operator=(const CJudgeTmpRouteEntry & src)
{
	m_HopFrom = src.m_HopFrom;
	m_HopDestinations.RemoveHead();
	POSITION pos = src.m_HopDestinations.GetHeadPosition();
	while (pos)
	{
		m_HopDestinations.AddTail(src.m_HopDestinations.GetNext(pos));
	}
	return *this;
}
