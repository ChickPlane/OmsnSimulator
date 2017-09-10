#pragma once
#include "DoublePoint.h"
#include "SimulatorCommon.h"

class CHostRouteEntry
{
public:
	CHostRouteEntry();
	CHostRouteEntry(const CDoublePoint & Position, SIM_TIME lnSimTime);
	~CHostRouteEntry();

	void SetValue(const CDoublePoint & Position, SIM_TIME lnSimTime);

	CDoublePoint m_Position;
	SIM_TIME m_lnSimTime;
};

