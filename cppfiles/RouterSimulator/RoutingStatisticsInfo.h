#pragma once
#include "MsgHopInfo.h"
#include "MsgInsideInfo.h"

class CRoutingStatisticsInfo
{
public:
	CRoutingStatisticsInfo();
	CRoutingStatisticsInfo(const CRoutingStatisticsInfo & src);
	CRoutingStatisticsInfo & operator = (const CRoutingStatisticsInfo & src);
	virtual ~CRoutingStatisticsInfo();

	void SetHopInfo(const CMsgInsideInfo & src);
	const CMsgInsideInfo & GetHopInfo() const;
	stCountAndPosition m_CnP[MSG_HOP_STATE_MAX];
protected:
	CMsgInsideInfo m_HopInfo;
};

