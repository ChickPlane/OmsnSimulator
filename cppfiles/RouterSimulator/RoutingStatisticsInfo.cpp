#include "stdafx.h"
#include "RoutingStatisticsInfo.h"


CRoutingStatisticsInfo::CRoutingStatisticsInfo()
{
}


CRoutingStatisticsInfo::CRoutingStatisticsInfo(const CRoutingStatisticsInfo & src)
{
	*this = src;
}

CRoutingStatisticsInfo & CRoutingStatisticsInfo::operator=(const CRoutingStatisticsInfo & src)
{
	m_HopInfo = src.m_HopInfo;
	return *this;
}

CRoutingStatisticsInfo::~CRoutingStatisticsInfo()
{
}

void CRoutingStatisticsInfo::SetHopInfo(const CMsgInsideInfo & src)
{
	m_HopInfo = src;
	m_HopInfo.GetCountAndIndex(m_CnP);
}

const CMsgInsideInfo & CRoutingStatisticsInfo::GetHopInfo() const
{
	return m_HopInfo;
}
