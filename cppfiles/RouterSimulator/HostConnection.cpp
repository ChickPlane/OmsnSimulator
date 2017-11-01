#include "stdafx.h"
#include "HostConnection.h"

CHostConnectionEntry::CHostConnectionEntry()
	: m_pNeighbor(NULL)
	, m_lnLastUpdate(0)
	, m_nUpdateTimes(0)
{

}


CHostConnectionEntry::CHostConnectionEntry(const CHostConnectionEntry & src)
{
	*this = src;
}

CHostConnectionEntry & CHostConnectionEntry::operator=(const CHostConnectionEntry & src)
{
	m_pNeighbor = src.m_pNeighbor;
	m_lnLastUpdate = src.m_lnLastUpdate;
	m_nUpdateTimes = src.m_nUpdateTimes;
	return *this;
}

CHostConnection::CHostConnection()
	:m_lnLastUpdate(0)
{
}


CHostConnection::~CHostConnection()
{
}

void CHostConnection::Reset(int nRecordCount)
{
	m_Records.RemoveAll();
	m_Records.SetSize(nRecordCount);
}
