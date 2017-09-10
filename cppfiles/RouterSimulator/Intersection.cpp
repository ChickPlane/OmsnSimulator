#include "stdafx.h"
#include "Intersection.h"
#include "RoadPoint.h"
#include "DijNode.h"
#include "Road.h"



CIntersection::CIntersection()
	:m_pPoint(NULL)
	, m_nId(-1)
{
}


CIntersection::CIntersection(PtrRoadPoint pPoint)
	:m_nId(-1)
{
	m_pPoint = pPoint;
}

CIntersection::~CIntersection()
{
}

void CIntersection::InsertNearbyPoints(int nDestinationPointId, int nNextPointId, double fDistance)
{
	CIntersectionRouter newRouter;
	int nNearbyCount = m_NearbyPoints.GetSize();
	for (int i = 0; i < nNearbyCount; ++i)
	{
		if (m_NearbyPoints[i].m_nDestinationPointId == nDestinationPointId)
		{
			if (m_NearbyPoints[i].m_fDistance < fDistance)
			{
				break;
			}
			m_NearbyPoints[i].SetValue(m_nId, nDestinationPointId, nNextPointId, fDistance);
			return;
		}
	}
	newRouter.SetValue(m_nId, nDestinationPointId, nNextPointId, fDistance);
	m_NearbyPoints.Add(newRouter);
}

CIntersectionRouter * CIntersection::GetNextPointRouter(int nDesPointId)
{
	int nLength = m_NearbyPoints.GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		if (m_NearbyPoints[i].m_nDestinationPointId == nDesPointId)
		{
			return &m_NearbyPoints[i];
		}
	}
	return NULL;
}

bool CIntersection::IsDijNode() const
{
	return true;
	if (m_pPoint->m_NearbyIntersections.GetSize() > 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CDijNode * CIntersection::SetDijState(int nID)
{
	if (IsDijNode())
	{
		if (m_pDij == NULL)
		{
			m_pDij = new CDijNode(this, nID);
		}
	}
	else
	{
		if (m_pDij != NULL)
		{
			delete m_pDij;
			m_pDij = NULL;
		}
	}
	return m_pDij;
}

CDijNode * CIntersection::GetDij()
{
	return m_pDij;
}

const CDijNode * CIntersection::GetDij() const
{
	return m_pDij;
}

void CIntersection::SetId(int nId)
{
	m_nId = nId;
}

int CIntersection::GetId()
{
	return m_nId;
}

CInterToInter & CIntersection::GetInterToInter(CIntersection * pDes)
{
	for (int i = 0; i < m_InterToInterConnects.GetSize(); ++i)
	{
		int nUpper = m_InterToInterConnects[i].m_Points.GetUpperBound();
		if (m_InterToInterConnects[i].m_Points[nUpper] == pDes->m_pPoint)
		{
			return m_InterToInterConnects[i];
		}
	}
	ASSERT(0);
}
