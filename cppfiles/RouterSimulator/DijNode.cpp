#include "stdafx.h"
#include "DijNode.h"
#include "Intersection.h"
#include "RoadPoint.h"



CDijNode::CDijNode()
	: m_pIntersection(NULL)
	, m_nID(-1)
{
}


CDijNode::CDijNode(CIntersection * pIntersection, int nID)
	: m_pIntersection(pIntersection)
	, m_nID(nID)
{

}

CDijNode::~CDijNode()
{
}

void CDijNode::AddNewEntry(CDijConnection & entry)
{
	CDijNode * a = entry.GetLastDijNode();
	int nLength = m_DijToDij.GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		CDijNode * b = m_DijToDij[i].GetLastDijNode();
		if (m_DijToDij[i].GetLastDijNode() == entry.GetLastDijNode())
		{
			if (m_DijToDij[i].m_fDistance > entry.m_fDistance)
			{
				m_DijToDij[i] = entry;
			}
			return;
		}
	}
	m_DijToDij.Add(entry);
}

void CDijNode::CreateDijConnections(CArray<PtrIntersection> & allInterSections)
{
	CRoadPoint * pPointStart = m_pIntersection->m_pPoint;
	int nSurroundingCount = pPointStart->m_NearbyIntersections.GetSize();
	for (int i = 0; i < nSurroundingCount; ++i)
	{
		CDijConnection newConn;
		int nInterId = pPointStart->m_NearbyIntersections[i].m_nIntersectionId;
		int nNextDijId = GetNextDijId(allInterSections, nInterId, newConn);
		if (nNextDijId != -1 && nNextDijId != m_nID)
		{
			AddNewEntry(newConn);
		}
	}
}

int CDijNode::GetNextDijId(CArray<PtrIntersection> & allInterSections, int nNextIntersectionId, CDijConnection & newConn)
{
	CRoadPoint * pPointStart = m_pIntersection->m_pPoint;
	PtrIntersection pNextIntersection = allInterSections[nNextIntersectionId];
	int nFromIntersectionId = m_pIntersection->GetId();
	bool bHaveNext = true;
	newConn.m_Intersections.RemoveAll();
	newConn.m_Intersections.Add(m_pIntersection);
	newConn.m_fDistance = 0;
	for (int i = 0; i < pPointStart->m_NearbyIntersections.GetSize(); ++i)
	{
		if (pPointStart->m_NearbyIntersections[i].m_nIntersectionId == nNextIntersectionId)
		{
			newConn.m_fDistance = pPointStart->m_NearbyIntersections[i].m_fDistance;
			break;
		}
	}
	ASSERT(newConn.m_fDistance != 0);
	newConn.m_Intersections.Add(pNextIntersection);
	while (bHaveNext && !pNextIntersection->IsDijNode())
	{
		bHaveNext = false;
		int nNearbyInterCount = pNextIntersection->m_pPoint->m_NearbyIntersections.GetSize();
		ASSERT(nNearbyInterCount <= 2);
		for (int i = 0; i < nNearbyInterCount; ++i)
		{
			int nNextInterId = pNextIntersection->m_pPoint->m_NearbyIntersections[i].m_nIntersectionId;
			if (nNextInterId != nFromIntersectionId)
			{
				nFromIntersectionId = pNextIntersection->GetId();
				newConn.m_fDistance += pNextIntersection->m_pPoint->m_NearbyIntersections[i].m_fDistance;

				pNextIntersection = allInterSections[nNextInterId];
				bHaveNext = true;
				newConn.m_Intersections.Add(pNextIntersection);
				break;
			}
		}
	}
	if (bHaveNext)
	{
		return pNextIntersection->GetDij()->m_nID;
	}
	else
	{
		return -1;
	}
}
