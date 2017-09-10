#include "stdafx.h"
#include "TrustEntry.h"


CTrustEntry::CTrustEntry()
{
}


CTrustEntry::CTrustEntry(const CTrustEntry & src)
{
	*this = src;
}

CTrustEntry & CTrustEntry::operator=(const CTrustEntry & src)
{
	m_nHostId = src.m_nHostId;
	m_fTrustValue = src.m_fTrustValue;
	return *this;
}

CTrustEntry::~CTrustEntry()
{
}
