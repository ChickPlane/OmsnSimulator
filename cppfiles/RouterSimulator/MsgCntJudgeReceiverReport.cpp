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
	POSITION pos = src.m_Items.GetStartPosition();
	CHost * rKey;
	CReceiverReportItem rValue;
	while (pos)
	{
		src.m_Items.GetNextAssoc(pos, rKey, rValue);
		m_Items[rKey] = rValue;
	}
	return *this;
}

CMsgCntJudgeReceiverReport::~CMsgCntJudgeReceiverReport()
{
}
