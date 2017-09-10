#include "stdafx.h"
#include "RouteTable.h"


CRouteTable::CRouteTable()
{
}


CRouteTable::CRouteTable(const CRouteTable & src)
{
	*this = src;
}

CRouteTable & CRouteTable::operator=(const CRouteTable & src)
{
	m_Items.RemoveAll();
	POSITION pos = src.m_Items.GetStartPosition();
	CRoutingProtocol * rKey;
	CRouteTableItem rValue;
	while (pos)
	{
		src.m_Items.GetNextAssoc(pos, rKey, rValue);
		m_Items[rKey] = rValue;
	}
	return *this;
}

CRouteTable::~CRouteTable()
{
}
