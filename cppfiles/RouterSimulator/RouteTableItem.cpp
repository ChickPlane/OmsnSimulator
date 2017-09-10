#include "stdafx.h"
#include "RouteTableItem.h"


CRouteTableItem::CRouteTableItem()
	: m_pNext(NULL)
	, m_pTo(NULL)
	, m_lnTimeOut(0)
{
}


CRouteTableItem::CRouteTableItem(const CRouteTableItem & src)
{
	*this = src;
}

CRouteTableItem & CRouteTableItem::operator=(const CRouteTableItem & src)
{
	m_pNext = src.m_pNext;
	m_pTo = src.m_pTo;
	m_lnTimeOut = src.m_lnTimeOut;
	return *this;
}

CRouteTableItem::~CRouteTableItem()
{
}
