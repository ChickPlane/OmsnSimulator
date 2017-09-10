#pragma once
#include "RouteTableItem.h"
class CRouteTable
{
public:
	CRouteTable();
	CRouteTable(const CRouteTable & src);
	CRouteTable & operator = (const CRouteTable & src);
	~CRouteTable();

	CMap<CRoutingProtocol *, CRoutingProtocol *, CRouteTableItem, CRouteTableItem&> m_Items;
};

