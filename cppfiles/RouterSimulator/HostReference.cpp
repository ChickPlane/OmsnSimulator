#include "stdafx.h"
#include "HostReference.h"


CHostReference::CHostReference()
{
}


CHostReference::CHostReference(const CHostReference & src)
{
	*this = src;
}

CHostReference & CHostReference::operator=(const CHostReference & src)
{
	m_Hosts.RemoveAll();
	POSITION pos = src.m_Hosts.GetHeadPosition();
	while (pos)
	{
		m_Hosts.AddTail(src.m_Hosts.GetNext(pos));
	}
	return *this;
}

CHostReference::~CHostReference()
{
}

void CHostReference::AddNewHost(CHostGui & newHost)
{
	m_Hosts.AddTail(newHost);
}
