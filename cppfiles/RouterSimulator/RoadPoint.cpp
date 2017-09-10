#include "stdafx.h"
#include "RoadPoint.h"
#include "Road.h"
#include "Intersection.h"


CRoadPoint::CRoadPoint()
{
	InitValues();
}


CRoadPoint::CRoadPoint(double x, double y)
	:CDoublePoint(x, y)
{
	InitValues();
}

CRoadPoint::CRoadPoint(const CDoublePoint & src)
	:CDoublePoint(src)
{
	InitValues();
}

CRoadPoint::~CRoadPoint()
{
}

void CRoadPoint::InitValues()
{
	m_pIntersection = NULL;
}

void CRoadPoint::InsertConnectedPoints(const CList<PtrRoadPoint> & newLine, POSITION pos)
{
	PtrRoadPoint pRecent = NULL;
	if (pos == NULL)
	{
		return;
	}
	pRecent = newLine.GetAt(pos);

	POSITION posNext = pos;
	PtrRoadPoint ptrNext = NULL;
	newLine.GetNext(posNext);
	if (posNext != NULL)
	{
		ptrNext = newLine.GetAt(posNext);
		ConnectTwoPoints(*pRecent, *ptrNext);
	}

	POSITION posPrev = pos;
	PtrRoadPoint ptrPrev = NULL;
	newLine.GetPrev(posPrev);
	if (posPrev != NULL)
	{
		ptrPrev = newLine.GetAt(posPrev);
		ConnectTwoPoints(*pRecent, *ptrPrev);
	}
}

void CRoadPoint::InsertConnectedPoint(PtrRoadPoint pConnected)
{
	if (!pConnected)
	{
		return;
	}
	int nConnectedPointCount = m_ConnectedPoints.GetSize();
	for (int i = 0; i < nConnectedPointCount; ++i)
	{
		if (pConnected == m_ConnectedPoints[i])
		{
			return;
		}
	}
	m_ConnectedPoints.Add(pConnected);
}

void CRoadPoint::ConnectTwoPoints(CRoadPoint & pointA, CRoadPoint & pointB)
{
	pointA.InsertConnectedPoint(&pointB);
	pointB.InsertConnectedPoint(&pointA);
}

void CRoadPoint::InsertNearbyIntersections(int nIntersectionId, int nNextPointId, double fDistance)
{
	CRoadRouter newRouter;
	int nNearbyCount = m_NearbyIntersections.GetSize();
	for (int i = 0; i < nNearbyCount; ++i)
	{
		if (m_NearbyIntersections[i].m_nIntersectionId == nIntersectionId)
		{
			if (m_NearbyIntersections[i].m_fDistance < fDistance)
			{
				break;
			}
			m_NearbyIntersections[i].SetValue(m_nId, nIntersectionId, nNextPointId, fDistance);
			return;
		}
	}
	newRouter.SetValue(m_nId, nIntersectionId, nNextPointId, fDistance);
	m_NearbyIntersections.Add(newRouter);
}

void CRoadPoint::SetId(int nId)
{
	m_nId = nId;
}

int CRoadPoint::GetId()
{
	return m_nId;
}

PtrIntersection CRoadPoint::InitIntersectionState()
{
	if (IsIntersection())
	{
		PtrIntersection pNewIntersection = SetAsIntersection(true);
	}
	else
	{
		SetAsIntersection(false);
	}
	return GetIntersection();
}

PtrIntersection CRoadPoint::SetAsIntersection(bool bSet)
{
	if (bSet)
	{
		if (!m_pIntersection)
		{
			m_pIntersection = new CIntersection(this);
		}
	}
	else
	{
		if (m_pIntersection)
		{
			delete m_pIntersection;
			m_pIntersection = NULL;
		}
	}
	return GetIntersection();
}

PtrIntersection CRoadPoint::GetIntersection()
{
	return m_pIntersection;
}

const PtrIntersection CRoadPoint::GetIntersection() const
{
	return m_pIntersection;
}

bool CRoadPoint::IsIntersection() const
{
	int nNeighbourCount = m_ConnectedPoints.GetSize();
	return nNeighbourCount > 2;
}

CRoadRouter * CRoadPoint::GetNextPointIdToIntersection(int nIntersectionId)
{
	for (int i = 0; i < m_NearbyIntersections.GetSize(); ++i)
	{
		if (m_NearbyIntersections[i].m_nIntersectionId == nIntersectionId)
		{
			return &m_NearbyIntersections[i];
		}
	}
	return NULL;
}

void CRoadPoint::GetRouteAgency(CArray<CRoadRouteAgency> & result, CArray<PtrIntersection> & m_allInterSections)
{
	result.RemoveAll();
	CRoadRouteAgency tmpAgency;
	tmpAgency.m_pRoadPoint = this;
	if (IsIntersection())
	{
		tmpAgency.m_fDistance = 0;
		tmpAgency.m_pIntersection = m_pIntersection;
		result.Add(tmpAgency);
		return;
	}
	int nIntersectionId = -1;
	for (int i = 0; i < m_NearbyIntersections.GetSize(); ++i)
	{
		tmpAgency.m_fDistance = m_NearbyIntersections[i].m_fDistance;
		nIntersectionId = m_NearbyIntersections[i].m_nIntersectionId;
		tmpAgency.m_pIntersection = m_allInterSections[nIntersectionId];
		result.Add(tmpAgency);
	}
}
