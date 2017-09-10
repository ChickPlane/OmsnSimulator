#pragma once
#include "HostGui.h"
class CMsgCntJudgeReceiverReport
{
public:
	CMsgCntJudgeReceiverReport();
	CMsgCntJudgeReceiverReport(const CMsgCntJudgeReceiverReport & src);
	CMsgCntJudgeReceiverReport & operator = (const CMsgCntJudgeReceiverReport & src);
	~CMsgCntJudgeReceiverReport();
	int m_nMsgId;
	CList<CHostGui> m_Hosts;
};

