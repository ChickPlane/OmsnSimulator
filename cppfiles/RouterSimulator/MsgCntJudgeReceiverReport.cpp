#include "stdafx.h"
#include "MsgCntJudgeReceiverReport.h"


CMsgCntJudgeReceiverReport::CMsgCntJudgeReceiverReport()
{
}


CMsgCntJudgeReceiverReport::CMsgCntJudgeReceiverReport(const CMsgCntJudgeReceiverReport & src)
{
	*this = src;
}

CMsgCntJudgeReceiverReport & CMsgCntJudgeReceiverReport::operator=(const CMsgCntJudgeReceiverReport & src)
{
	m_nMsgId = src.m_nMsgId;
	POSITION pos = src.m_Hosts.GetHeadPosition();
	while (pos)
	{
		m_Hosts.AddTail(src.m_Hosts.GetNext(pos));
	}
	return *this;
}

CMsgCntJudgeReceiverReport::~CMsgCntJudgeReceiverReport()
{
}
