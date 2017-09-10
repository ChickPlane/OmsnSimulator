#include "stdafx.h"
#include "RoutingData.h"
#include "Host.h"


CRoutingData::CRoutingData()
	: m_pHostFrom(NULL)
	, m_pHostTo(NULL)
	, m_pProtocolSrc(NULL)
	, m_pProtocolDst(NULL)
	, m_lnTimeOut(0)
{
	ChangeDataId();
}


CRoutingData::CRoutingData(const CRoutingData & src)
{
	*this = src;
}

CRoutingData & CRoutingData::operator=(const CRoutingData & src)
{
	m_nDataId = src.m_nDataId;
	m_pHostFrom = src.m_pHostFrom;
	m_pHostTo = src.m_pHostTo;
	m_pProtocolSrc = src.m_pProtocolSrc;
	m_pProtocolDst = src.m_pProtocolDst;
	m_lnTimeOut = src.m_lnTimeOut;
	m_Statistic = src.m_Statistic;
	return *this;
}

CRoutingData::~CRoutingData()
{
}

CRoutingData * CRoutingData::GetDuplicate() const
{
	CRoutingData * pRet = new CRoutingData(*this);
	return pRet;
}

void CRoutingData::ChangeDataId()
{
	m_nDataId = IncreaseDataId();
}

void CRoutingData::SetValue(CHost * pHostFrom, CHost * pHostTo, SIM_TIME lnTimeOut)
{
	ASSERT(pHostFrom != NULL);
	ASSERT(pHostTo != NULL);
	m_pHostFrom = pHostFrom;
	m_pHostTo = pHostTo;
	ASSERT(m_pHostFrom->m_pProtocol);
	m_pProtocolSrc = m_pHostFrom->m_pProtocol;
	ASSERT(m_pHostTo->m_pProtocol);
	m_pProtocolDst = m_pHostTo->m_pProtocol;
	m_lnTimeOut = lnTimeOut;
}

int CRoutingData::GetDataId(const CHost * pReader) const
{
	return m_nDataId;
}

CHost * CRoutingData::GetHostFrom(const CHost * pReader) const
{
	return m_pHostFrom;
}

CHost * CRoutingData::GetHostTo(const CHost * pReader) const
{
	return m_pHostTo;
}

SIM_TIME CRoutingData::GetTimeOut(const CHost * pReader) const
{
	return m_lnTimeOut;
}

void CRoutingData::RecordHop(const CMsgHopInfo & info)
{
	m_Statistic.RecordInfo(info);
}

int CRoutingData::sm_nDataIdMax = 0;

int CRoutingData::IncreaseDataId()
{
	++sm_nDataIdMax;
	return sm_nDataIdMax;
}
