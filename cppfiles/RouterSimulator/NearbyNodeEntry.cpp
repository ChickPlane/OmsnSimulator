#include "stdafx.h"
#include "NearbyNodeEntry.h"


CNearbyNodeEntry::CNearbyNodeEntry()
	: m_lnTimeOut(0)
	, m_pProtocol(NULL)
{
}

CNearbyNodeEntry::CNearbyNodeEntry(const CNearbyNodeEntry & src)
{
	*this = src;
}

CNearbyNodeEntry & CNearbyNodeEntry::operator=(const CNearbyNodeEntry & src)
{
	m_lnTimeOut = src.m_lnTimeOut;
	m_pProtocol = src.m_pProtocol;
	return *this;
}

CNearbyNodeEntry::~CNearbyNodeEntry()
{
}
