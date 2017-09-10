#include "stdafx.h"
#include "StatisticsReport.h"


CStatisticsReport::CStatisticsReport()
{
}


CStatisticsReport::CStatisticsReport(const CStatisticsReport & src)
{
	*this = src;
}

CStatisticsReport::~CStatisticsReport()
{
}

CStatisticsReport & CStatisticsReport::operator=(const CStatisticsReport & src)
{
	m_nStartedPackages = src.m_nStartedPackages;
	m_nDeliveredPackages = src.m_nDeliveredPackages;
	m_nStartAnonyCount = src.m_nStartAnonyCount;
	m_nFinishAnonyCount = src.m_nFinishAnonyCount;

	m_fAveLatency = src.m_fAveLatency;
	m_fAveTotalHops = src.m_fAveTotalHops;
	m_fAveAnonyTimeCost = src.m_fAveAnonyTimeCost;
	return *this;
}
