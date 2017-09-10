#pragma once
#include "DoublePoint.h"
#include "HostGui.h"

class CHost;

class CHostReference
{
public:
	CHostReference();
	CHostReference(const CHostReference & src);
	CHostReference & operator = (const CHostReference & src);
	~CHostReference();

	void AddNewHost(CHostGui & newHost);

	CList<CHostGui> m_Hosts;
};

