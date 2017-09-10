#include "stdafx.h"
#include "HostRouteTable.h"


CHostRouteTable::CHostRouteTable()
	: m_fStartSecond(0.0)
{
}


CHostRouteTable & CHostRouteTable::operator=(const CHostRouteTable & src)
{
	m_fStartSecond = src.m_fStartSecond;
	m_Entries.Copy(src.m_Entries);
	return *this;
}

CHostRouteTable::~CHostRouteTable()
{
}

void CHostRouteTable::InsertOneEntry(const CDoublePoint & Position, SIM_TIME lnSimTimeCost)
{
	int nUpperBound = m_Entries.GetUpperBound();
	if (nUpperBound == -1)
	{
		m_Entries.Add(CHostRouteEntry(Position, lnSimTimeCost));
	}
	else
	{
		SIM_TIME lnLastSimTime = m_Entries.GetAt(nUpperBound).m_lnSimTime;
		m_Entries.Add(CHostRouteEntry(Position, lnLastSimTime + lnSimTimeCost));
	}
}

double CHostRouteTable::InsertOneEntryWithSpeed(const CDoublePoint & Position, double fSpeed)
{
	int nUpperBound = m_Entries.GetUpperBound();
	if (nUpperBound == -1)
	{
		m_Entries.Add(CHostRouteEntry(Position, 0));
		return 0.0;
	}
	else
	{
		SIM_TIME lnLastSimTime = m_Entries.GetAt(nUpperBound).m_lnSimTime;
		double fDistance = CDoublePoint::GetDistance(m_Entries.GetAt(nUpperBound).m_Position, Position);
		SIM_TIME lnSimTimeCost = 1000.0 * fDistance / fSpeed;
		m_Entries.Add(CHostRouteEntry(Position, lnLastSimTime + lnSimTimeCost));
		return fDistance;
	}
}

void CHostRouteTable::Reset()
{
	m_fStartSecond = 0.0;
	m_Entries.RemoveAll();
}

CDoublePoint CHostRouteTable::GetPosition(SIM_TIME lnSimTime) const
{
	INT_PTR nFrom = 0;
	INT_PTR nTo = m_Entries.GetUpperBound();
	if (nTo == 0)
	{
		return m_Entries[nTo].m_Position;
	}
	SIM_TIME lnTimeFrom = m_Entries[nFrom].m_lnSimTime;
	SIM_TIME lnTimeTo = m_Entries[nTo].m_lnSimTime;
	SIM_TIME lnScheduleLength = lnTimeTo - lnTimeFrom;
	ASSERT(lnScheduleLength != 0);
	int fMulriple = lnSimTime / lnScheduleLength;
	lnSimTime = lnSimTime % lnScheduleLength;
	if (fMulriple % 2 == 1)
	{
		lnSimTime = lnScheduleLength - lnSimTime;
	}
	if (lnSimTime <= m_Entries[nFrom].m_lnSimTime)
	{
		return m_Entries[nFrom].m_Position;
	}
	if (lnSimTime >= m_Entries[nTo].m_lnSimTime)
	{
		return m_Entries[nTo].m_Position;
	}
	INT_PTR nMid = (nFrom + nTo) / 2;
	while (m_Entries[nMid].m_lnSimTime != lnSimTime && nTo - nFrom > 1)
	{
		if (m_Entries[nMid].m_lnSimTime > lnSimTime)
		{
			nTo = nMid;
		}
		else
		{
			nFrom = nMid;
		}
		nMid = (nFrom + nTo) / 2;
	}
	if (m_Entries[nMid].m_lnSimTime == lnSimTime)
	{
		return m_Entries[nMid].m_Position;
	}
	else
	{
		double fTimeDiffer = m_Entries[nTo].m_lnSimTime - m_Entries[nFrom].m_lnSimTime;
		double fRate = (lnSimTime - m_Entries[nFrom].m_lnSimTime) / fTimeDiffer;
		CDoublePoint ret;
		ret.m_X = m_Entries[nFrom].m_Position.m_X + (m_Entries[nTo].m_Position.m_X - m_Entries[nFrom].m_Position.m_X) * fRate;
		ret.m_Y = m_Entries[nFrom].m_Position.m_Y + (m_Entries[nTo].m_Position.m_Y - m_Entries[nFrom].m_Position.m_Y) * fRate;
		return ret;
	}
}

bool CHostRouteTable::IsScheduleAvailable()
{
	return m_Entries.GetSize() > 0;
}
