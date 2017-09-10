#include "stdafx.h"
#include "RoutingDataEnc.h"


CRoutingDataEnc::CRoutingDataEnc()
	: m_bIsEncrypted(false)
{
}


CRoutingDataEnc::CRoutingDataEnc(const CRoutingDataEnc & src)
	: CRoutingData(src)
{

}

CRoutingDataEnc & CRoutingDataEnc::operator=(const CRoutingDataEnc & src)
{
	CRoutingData::operator=(src);
	m_bIsEncrypted = src.m_bIsEncrypted;
	return *this;
}

CRoutingDataEnc::~CRoutingDataEnc()
{
}

CRoutingDataEnc * CRoutingDataEnc::GetDuplicate() const
{
	CRoutingDataEnc * pRet = new CRoutingDataEnc(*this);
	return pRet;
}

int CRoutingDataEnc::GetDataId(const CHost * pReader) const
{
	if (!CanRead(pReader))
	{
		return INVALID_DATA_ID;
	}
	return ForceGetDataId(pReader);
}

CHost * CRoutingDataEnc::GetHostFrom(const CHost * pReader) const
{
	if (!CanRead(pReader))
	{
		return NULL;
	}
	return ForceGetHostFrom(pReader);
}

CHost * CRoutingDataEnc::GetHostTo(const CHost * pReader) const
{
	if (!CanRead(pReader))
	{
		return NULL;
	}
	return ForceGetHostTo(pReader);
}

SIM_TIME CRoutingDataEnc::GetTimeOut(const CHost * pReader) const
{
	if (!CanRead(pReader))
	{
		return 0;
	}
	return ForceGetTimeOut(pReader);
}

bool CRoutingDataEnc::CanRead(const CHost * pReader) const
{
	if (m_bIsEncrypted)
	{
		return (m_pHostTo == pReader);
	}
	else
	{
		return true;
	}
}

void CRoutingDataEnc::SetEncrypted(bool bEnc)
{
	m_bIsEncrypted = bEnc;
}

int CRoutingDataEnc::ForceGetDataId(const CHost * pReader) const
{
	return CRoutingData::GetDataId(pReader);
}

CHost * CRoutingDataEnc::ForceGetHostFrom(const CHost * pReader) const
{
	return CRoutingData::GetHostFrom(pReader);
}

CHost * CRoutingDataEnc::ForceGetHostTo(const CHost * pReader) const
{
	return CRoutingData::GetHostTo(pReader);
}

SIM_TIME CRoutingDataEnc::ForceGetTimeOut(const CHost * pReader) const
{
	return CRoutingData::GetTimeOut(pReader);
}
