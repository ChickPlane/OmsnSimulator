#include "stdafx.h"
#include "HostInfo.h"


CHostInfo::CHostInfo()
{
}


CHostInfo::~CHostInfo()
{
}

void CHostInfo::Reset()
{
	m_nHostId = 0;
	m_protocolInfo.Reset();
}
