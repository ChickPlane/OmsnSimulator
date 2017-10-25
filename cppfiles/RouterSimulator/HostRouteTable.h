#pragma once
#include "HostRouteEntry.h"
class CHostRouteTable
{
public:
	CHostRouteTable();
	CHostRouteTable & operator = (const CHostRouteTable & src);
	CHostRouteTable & operator += (const CHostRouteTable & src);
	~CHostRouteTable();
	void InsertOneEntry(const CDoublePoint & Position, SIM_TIME lnSimTimeCost);
	double InsertOneEntryWithSpeed(const CDoublePoint & Position, double fSpeed);
	void Reset();
	CDoublePoint GetPosition(SIM_TIME lnSimTime) const;
	bool IsScheduleAvailable() const;

	double m_fStartSecond;
	CArray<CHostRouteEntry> m_Entries;

};

