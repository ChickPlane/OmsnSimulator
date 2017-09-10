#include "stdafx.h"
#include "Road.h"
#include "RoadPoint.h"
#include "Intersection.h"



CRoad::CRoad()
{
}


CRoad::~CRoad()
{
}

void CRoad::Init(const CList<PtrRoadPoint> & newLine)
{
	//m_Points.SetSize(newLine.GetSize());
	POSITION pos = newLine.GetHeadPosition();
	PtrRoadPoint currentPoint;
	while (pos != NULL)
	{
		currentPoint = newLine.GetNext(pos);
		m_Points.Add(currentPoint);
	}
}

void CRoad::ReversePoints()
{
	int nLength = m_Points.GetSize();
	PtrRoadPoint tmpPointPtr;
	for (int i = 0; i < nLength/2; ++i)
	{
		tmpPointPtr = m_Points[i];
		m_Points[i] = m_Points[nLength - i - 1];
		m_Points[nLength - i - 1] = tmpPointPtr;
	}
}

void CRoad::ReceivePointsFrom(CRoad * pNewRoad)
{
	ASSERT(m_Points[m_Points.GetSize() - 1] == pNewRoad->m_Points[0]);
	for (int i = 1; i < pNewRoad->m_Points.GetSize(); ++i)
	{
		m_Points.Add(pNewRoad->m_Points[i]);
	}
}

int CRoad::GetNeighbours(PtrRoadPoint pTarget, CList<PtrRoadPoint> & result)
{
	int ret = 0;
	int nLength = m_Points.GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		PtrRoadPoint pRecent = m_Points[i];
		if (pRecent == pTarget)
		{
			if (i > 0)
			{
				PtrRoadPoint pInsert = m_Points[i - 1];
				if (result.Find(pInsert) == NULL)
				{
					result.AddTail(pInsert);
					++ret;
				}
			}
			if (i < nLength - 1)
			{
				PtrRoadPoint pInsert = m_Points[i + 1];
				if (result.Find(pInsert) == NULL)
				{
					result.AddTail(pInsert);
					++ret;
				}
			}
		}
	}
	return ret;
}
