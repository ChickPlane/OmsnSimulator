#include "stdafx.h"
#include "HostRouteEntry.h"


CHostRouteEntry::CHostRouteEntry()
	: m_lnSimTime(-1)
{
}


CHostRouteEntry::CHostRouteEntry(const CDoublePoint & Position, SIM_TIME lnSimTime)
{
	SetValue(Position, lnSimTime);
}

CHostRouteEntry::~CHostRouteEntry()
{
}

void CHostRouteEntry::SetValue(const CDoublePoint & Position, SIM_TIME lnSimTime)
{
	m_Position = Position;
	m_lnSimTime = lnSimTime;
}
