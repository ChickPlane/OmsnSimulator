#pragma once
#include "SimulatorCommon.h"

class CRoutingProtocol;

class CRouteTableItem
{
public:
	CRouteTableItem();
	CRouteTableItem(const CRouteTableItem & src);
	CRouteTableItem & operator = (const CRouteTableItem & src);
	~CRouteTableItem();

	CRoutingProtocol * m_pNext;
	CRoutingProtocol * m_pTo;
	SIM_TIME m_lnTimeOut;
};

