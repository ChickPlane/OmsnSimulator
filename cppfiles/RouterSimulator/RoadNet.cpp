#include "stdafx.h"
#include "RoadNet.h"
#include "RoadPoint.h"
#include "Road.h"
#include "Intersection.h"
#include "streethead.h"
#include "DijNode.h"
#include <fstream>
#include "DijItem.h"
#include <string>

#include "PointReference.h"
#include "WktParse.h"
using namespace std;

CRoadNet::CRoadNet()
	:m_pDijTable(NULL)
{
}


CRoadNet::~CRoadNet()
{
	POSITION pos = m_Reference.GetStartPosition();
	CPointReference * pReference = NULL;
	int nHash = 0;
	while (pos)
	{
		m_Reference.GetNextAssoc(pos, nHash, pReference);
		delete pReference;
	}
	m_Reference.RemoveAll();

	for (int i = 0; i < m_allPoints.GetSize(); ++i)
	{
		delete m_allPoints[i];
	}
	m_allPoints.RemoveAll();

	for (int i = 0; i < m_allHosts.GetSize(); ++i)
	{
		delete m_allHosts[i];
	}
	m_allHosts.RemoveAll();
}

bool CRoadNet::HasHash(const CDoublePoint & testPoint) const
{
	if (testPoint.m_X < m_lefttop.m_X || testPoint.m_X > m_rightbottom.m_X)
		return false;
	if (testPoint.m_Y < m_lefttop.m_Y || testPoint.m_Y > m_rightbottom.m_Y)
		return false;
	return true;
}

void CRoadNet::Init(const CList<CLineInMap>& lines, double & l, double & r, double & t, double b)
{
	m_lefttop.m_X = l;
	m_lefttop.m_Y = t;
	m_rightbottom.m_X = r;
	m_rightbottom.m_Y = b;
	if (m_PointHash.SetValue(m_lefttop, m_rightbottom, 100) != 100)
	{
		ASSERT(false);
	}

	POSITION pos = lines.GetHeadPosition();
	int nPointCount = 0;
	while (pos != NULL)
	{
		CList<PtrRoadPoint> newLine;
		newLine.RemoveAll();

		CLineInMap tmpLine = lines.GetNext(pos);
		for (int i = 0; i < tmpLine.m_Line.GetSize(); ++i)
		{
			PtrRoadPoint newPoint = FindRoadPoint(tmpLine.m_Line.GetAt(i));
			
			if (newPoint == NULL)
			{
				newPoint = new CRoadPoint(tmpLine.m_Line.GetAt(i));
				newPoint->SetId(nPointCount++);
				AddNewRoadPoint(newPoint);
				ASSERT(m_allPoints.GetSize() == nPointCount);
			}
			newLine.AddTail(newPoint);
		}
		AddOneRoad(newLine);
		//ASSERT(CheckInterHasTwoRoads());
	}
	SplitLongStraightRoads(30,3);
	DetermineAllInterSections();
	DetermineAllRoadInsideRoute();

	InitOriginalDistances();

	DetermineAllDij();
	SetAllDijDistance();
	BuildAllInterToInter();
}

void CRoadNet::AddOneRoad(const CList<PtrRoadPoint> & newLine)
{
	POSITION pos = newLine.GetHeadPosition();
	while (pos != NULL)
	{
		CRoadPoint::InsertConnectedPoints(newLine, pos);
		newLine.GetNext(pos);
	}
}

void CRoadNet::DetermineAllInterSections()
{
	int nPointCount = m_allPoints.GetSize();
	int nIntersectionIndex = 0;
	int nIntersectionCount = 0;
	for (int i = 0; i < nPointCount; ++i)
	{
		PtrIntersection pNewIntersection = m_allPoints[i]->InitIntersectionState();
		if (pNewIntersection) 
		{
			pNewIntersection->SetId(nIntersectionCount++);
			m_allInterSections.Add(pNewIntersection);
		}
	}
}

void CRoadNet::DetermineAllRoadInsideRoute()
{
	int nPointCount = m_allPoints.GetSize();
	int nIntersectionIndex = 0;
	int nIntersectionCount = 0;
	for (int i = 0; i < nPointCount; ++i)
	{
		int nRoadCount = m_allPoints[i]->m_ConnectedPoints.GetSize();
		CRoadPoint * pStart = m_allPoints[i];
		for (int j = 0; j < nRoadCount; ++j)
		{
			CRoadPoint * pPrev = pStart;
			CRoadPoint * pFirstNext = m_allPoints[i]->m_ConnectedPoints[j];
			CRoadPoint * pNext = pFirstNext;
			int nSrcId = pStart->GetId();
			int nNextId = pFirstNext->GetId();
			int nDesId = pNext->GetId();
			double fDistance = 0;
			int nChoiceCount = pNext->m_ConnectedPoints.GetSize();
			while(true)
			{
				if (pNext == pStart)
				{
					break;
				}
				nDesId = pNext->GetId();
				fDistance += CDoublePoint::GetDistance(*pPrev, *pNext);
				pStart->m_InsideRouter.InsertItem(nSrcId, nNextId, nDesId, fDistance);
				nChoiceCount = pNext->m_ConnectedPoints.GetSize();
				if (nChoiceCount != 2)
				{
					break;
				}
				if (pNext->m_ConnectedPoints[0]->GetId() == pPrev->GetId())
				{
					pPrev = pNext;
					pNext = pNext->m_ConnectedPoints[1];
				}
				else
				{
					ASSERT(pNext->m_ConnectedPoints[1]->GetId() == pPrev->GetId());
					pPrev = pNext;
					pNext = pNext->m_ConnectedPoints[0];
				}
			}
		}
	}
}

void CRoadNet::DetermineAllDij()
{
	int nInterCount = m_allInterSections.GetSize();
	int nDijId = 0;
	for (int i = 0; i < nInterCount; ++i)
	{
		CDijNode * pNewDij = m_allInterSections[i]->SetDijState(nDijId);
		if (pNewDij == NULL)
		{
			continue;
		}
		++nDijId;
		m_allDijNodes.Add(pNewDij);
	}
}

void CRoadNet::SetAllDijDistance()
{
	int nDij = m_allDijNodes.GetSize();
	for (int i = 0; i < nDij; ++i)
	{
		//ASSERT(i != 20 && i != 21);
		m_allDijNodes[i]->CreateDijConnections(m_allInterSections);
	}
}

void CRoadNet::InitOriginalDistances()
{
	int nInterCOunt = m_allInterSections.GetSize();
	for (int i = 0; i < nInterCOunt; ++i)
	{
		InitOriginalFromIntersectionStart(i);
	}
}

void CRoadNet::InitOriginalFromIntersectionStart(int nIntersectionId)
{
	PtrIntersection pCurrentIntersection = m_allInterSections[nIntersectionId];
	PtrRoadPoint pIntersectionPoint = pCurrentIntersection->m_pPoint;
	int nConnectCount = pIntersectionPoint->m_ConnectedPoints.GetSize();
	double fDistance = 0;
	for (int i = 0; i < nConnectCount; ++i)
	{
		PtrRoadPoint pNext = pIntersectionPoint->m_ConnectedPoints[i];
		fDistance = CDoublePoint::GetDistance(*pNext, *pIntersectionPoint);
		InitOriginalFromIntersection(nIntersectionId, pNext, pIntersectionPoint, pNext, fDistance);
	}
}

void CRoadNet::InitOriginalFromIntersection(int nIntersectionId, PtrRoadPoint pIntersectionNext, PtrRoadPoint pDesPrev, PtrRoadPoint pDes, double fDistance)
{
	ASSERT(pDes != NULL);

	PtrRoadPoint pIntersectionPoint = m_allInterSections[nIntersectionId]->m_pPoint;
	PtrIntersection pCurrentIntersection = m_allInterSections[nIntersectionId];

	pCurrentIntersection->InsertNearbyPoints(pDes->GetId(), pIntersectionNext->GetId(), fDistance);
	pDes->InsertNearbyIntersections(nIntersectionId, pDesPrev->GetId(), fDistance);

	if (pDes->IsIntersection())
	{
		int nNextInterId = pDes->GetIntersection()->GetId();
		pIntersectionPoint->InsertNearbyIntersections(nNextInterId, pIntersectionNext->GetId(), fDistance);
	}
	else
	{
		int nConnectCount = pDes->m_ConnectedPoints.GetCount();
		for (int i = 0; i < nConnectCount; ++i)
		{
			PtrRoadPoint pNext = pDes->m_ConnectedPoints[i];
			if (pNext == pDesPrev)
			{
				continue;
			}
			double fDistanceToNext = CDoublePoint::GetDistance(*pDes, *pNext);
			InitOriginalFromIntersection(nIntersectionId, pIntersectionNext, pDes, pNext, fDistanceToNext + fDistance);
		}
	}
}

DWORD CRoadNet::GetHashValue(const CDoublePoint * pPoint)
{
	return m_PointHash.GetHashValue(pPoint);
}

DWORD CRoadNet::GetHashValue(double fX, double fY)
{
	return m_PointHash.GetHashValue(fX, fY);
}

double CRoadNet::GetHashInterval() const
{
	return m_PointHash.GetInterval();
}

void CRoadNet::CalculateBlockAndPredictTime(int & nBlockCount, SIM_TIME & lnPredictTime, SIM_TIME & lnHalfBlockTime, double fCommunicationRadius) const
{
	double fHashInterval = GetHashInterval();
	double fSpeedMax = GetSpeedLimit();
	int nBlock = fCommunicationRadius / fHashInterval;
	nBlockCount = nBlock + 1;
	lnPredictTime = 1000 * (nBlockCount * fHashInterval - fCommunicationRadius) / (2 * fSpeedMax);
	lnHalfBlockTime = 1000 * fHashInterval / (2 * fSpeedMax);
}

void CRoadNet::GetMapRange(CDoublePoint & lefttop, CDoublePoint & rightbottom)
{
	lefttop.m_X = DBL_MAX;
	lefttop.m_Y = DBL_MAX;
	rightbottom.m_X = 0;
	rightbottom.m_Y = 0;
	for (int i = 0; i < m_allPoints.GetSize(); ++i)
	{
		PtrRoadPoint tmpPoint = m_allPoints[i];
		if (tmpPoint->m_X > rightbottom.m_X)
		{
			rightbottom.m_X = tmpPoint->m_X;
		}
		if (tmpPoint->m_Y > rightbottom.m_Y)
		{
			rightbottom.m_Y = tmpPoint->m_Y;
		}
		if (tmpPoint->m_X < lefttop.m_X)
		{
			lefttop.m_X = tmpPoint->m_X;
		}
		if (tmpPoint->m_Y < lefttop.m_Y)
		{
			lefttop.m_Y = tmpPoint->m_Y;
		}
		
	}
}

void CRoadNet::GetMapCenter(CDoublePoint & center)
{
	if (m_allPoints.GetSize() == 0)
	{
		return;
	}
	center.m_X = 0;
	center.m_Y = 0;
	if (m_allInterSections.GetSize() > 0)
	{
		for (int i = 0; i < m_allInterSections.GetSize(); ++i)
		{
			PtrRoadPoint tmpPoint = m_allInterSections[i]->m_pPoint;
			center.m_X += tmpPoint->m_X;
			center.m_Y += tmpPoint->m_Y;
		}
		center.m_X /= m_allInterSections.GetSize();
		center.m_Y /= m_allInterSections.GetSize();
	}
	else
	{
		for (int i = 0; i < m_allPoints.GetSize(); ++i)
		{
			if (!m_allPoints[i]->IsIntersection())
			{
				continue;
			}
			PtrRoadPoint tmpPoint = m_allPoints[i];
			center.m_X += tmpPoint->m_X;
			center.m_Y += tmpPoint->m_Y;
		}
		center.m_X /= m_allPoints.GetSize();
		center.m_Y /= m_allPoints.GetSize();
	}
}

void CRoadNet::AddNewRoadPoint(PtrRoadPoint pRoadPoint)
{
	ASSERT(m_allPoints.GetSize() == pRoadPoint->GetId());
	m_allPoints.Add(pRoadPoint);
	DWORD nHash = m_PointHash.GetHashValue(pRoadPoint);
	CPointReference * ptr_pointreference = NULL;
	if (!m_Reference.Lookup(nHash, ptr_pointreference))
	{
		ptr_pointreference = new CPointReference();
		m_Reference[nHash] = ptr_pointreference;
	}
	ptr_pointreference->AddNewPoint(pRoadPoint);
}

PtrRoadPoint CRoadNet::FindRoadPoint(const CDoublePoint & coor)
{
	DWORD nHash = m_PointHash.GetHashValue(&coor);
	CPointReference * pReference = NULL;
	if (m_Reference.Lookup(nHash, pReference))
	{
		return (PtrRoadPoint)pReference->FindPointCoor(coor);
	}
	else
	{
		return NULL;
	}
}

PtrRoadPoint CRoadNet::FindNearestRoadPoints(const CDoublePoint & coor, double fDistance)
{
	double fInterval = m_PointHash.GetInterval();
	int nBlockCount = fDistance / fInterval + 1;
	int nHash = 0;
	CPointReference * pReference = NULL;
	double fMin = DBL_MAX;
	PtrRoadPoint pResult = NULL;
	CDoublePoint tmpPoint;
	for (int i = -nBlockCount; i <= nBlockCount; ++i)
	{
		for (int j = -nBlockCount; j <= nBlockCount; ++j)
		{
			tmpPoint.SetValue(coor.m_X + i * fInterval, coor.m_Y + j * fInterval);
			nHash = m_PointHash.GetHashValue(&tmpPoint);
			if (!m_Reference.Lookup(nHash, pReference))
			{
				continue;
			}
			CDoublePoint * pTmpResult = pReference->FindNearest(tmpPoint);
			double fTmpDistance = CDoublePoint::GetDistance(*pTmpResult, tmpPoint);
			if(fTmpDistance < fMin)
			{
				fMin = fTmpDistance;
				pResult = (PtrRoadPoint)pTmpResult;
			}
		}
	}
	return pResult;
}

PtrRoadPoint CRoadNet::FindNearestRoadPoint(const CDoublePoint & coor)
{
	double fInterval = m_PointHash.GetInterval();
	int nHash = 0;
	CPointReference * pReference = NULL;
	double fMin = DBL_MAX;
	PtrRoadPoint pResult = NULL;
	CDoublePoint tmpPoint;
	int nBlockCountMax = (unsigned short)USHORT_MAX + 1;
	int nBlockLength = 0;
	while (nBlockLength <= nBlockCountMax)
	{
		for (int i = -nBlockLength; i <= nBlockLength; ++i)
		{
			for (int j = -nBlockLength; j <= nBlockLength; ++j)
			{
				tmpPoint.SetValue(coor.m_X + i * fInterval, coor.m_Y + j * fInterval);
				nHash = m_PointHash.GetHashValue(&tmpPoint);
				if (!m_Reference.Lookup(nHash, pReference))
				{
					continue;
				}
				CDoublePoint * pTmpResult = pReference->FindNearest(coor);
				double fTmpDistance = CDoublePoint::GetDistance(*pTmpResult, coor);
				if (fTmpDistance < fMin)
				{
					fMin = fTmpDistance;
					pResult = (PtrRoadPoint)pTmpResult;
					nBlockCountMax = fMin / fInterval + 1;
				}
			}
		}
		++nBlockLength;
	}
	return pResult;
}

int CRoadNet::GetHostNumberInRange(const CDoublePoint & coor, double fRadius, SIM_TIME lnTime) const
{
	int nRet = 0;
	for (int i = 0; i < m_allHosts.GetSize(); ++i)
	{
		CDoublePoint dpPosition = m_allHosts[i]->GetPosition(lnTime);
		double fDistance = CDoublePoint::GetDistance(dpPosition, coor);
		if (fDistance <= fRadius)
		{
			nRet++;
		}
	}
	return nRet;
}

double CRoadNet::GetSpeedLimit() const
{
	return 33.3333;
}

void CRoadNet::LoadBuildingSettings(char * strFileName)
{
	CList<CBuilding> buildList;
	int nBuildingCount = CWktParse::ParseBuildings(strFileName, m_lefttop.m_X, m_rightbottom.m_X, m_lefttop.m_Y, m_rightbottom.m_Y, buildList);
	m_allBuildings.SetSize(nBuildingCount);
	POSITION pos = buildList.GetHeadPosition();
	int i = 0;
	while (pos)
	{
		CBuilding * pNew = new CBuilding();
		*pNew = buildList.GetNext(pos);
		pNew->m_nId = i;
		m_allBuildings[i++] = pNew;
	}
}

void CRoadNet::ConnectBuildingsToRoads()
{
	int nLength = m_allBuildings.GetCount();
	PtrRoadPoint pNearest = NULL;
	for (int i = 0; i < nLength; ++i)
	{
		pNearest = FindNearestRoadPoint(*m_allBuildings[i]);
		ASSERT(pNearest != NULL);
		ConnectBuildAndRoad(m_allBuildings[i], pNearest);
	}
}

void CRoadNet::BuildAllInterToInter()
{
	int nLength = m_allInterSections.GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		BuildInterToInter(m_allInterSections[i]);
	}
}

void CRoadNet::BuildInterToInter(CIntersection * pSrc)
{
	int nLength = pSrc->m_pPoint->m_NearbyIntersections.GetSize();
	CInterToInter interToInter;
	for (int i = 0; i < nLength; ++i)
	{
		int nIntersectionId = pSrc->m_pPoint->m_NearbyIntersections[i].m_nIntersectionId;
		CIntersection * pDes = m_allInterSections[nIntersectionId];
		GetAllPointsBetweenIntersections(interToInter, pSrc, pDes);
		pSrc->m_InterToInterConnects.Add(interToInter);
	}
}

void CRoadNet::GetAllPointsBetweenIntersections(CInterToInter & result, CIntersection * pSrc, CIntersection * pDes)
{
	result.m_Points.RemoveAll();
	CRoadRouter * pRoadRouter = pSrc->m_pPoint->GetNextPointIdToIntersection(pDes->GetId());
	result.m_fDistance = pRoadRouter->m_fDistance;
	CRoadPoint * pRecent = pSrc->m_pPoint;
	while (pRecent != pDes->m_pPoint)
	{
		result.m_Points.Add(pRecent);
		pRoadRouter = pRecent->GetNextPointIdToIntersection(pDes->GetId());
		int nNextId = pRoadRouter->m_nNextPointId;
		ASSERT(nNextId != -1);
		pRecent = m_allPoints[nNextId];
	}
	result.m_Points.Add(pDes->m_pPoint);
}

void CRoadNet::WalkAlongRoad(CHostRouteTable & hostRoute, CIntersection * pSrc, CRoadPoint * pNext, CRoadPoint * pDes, double fSpeed)
{
	CRoadPoint * pRecent = pNext;
	CRoadPoint * pPrev = pSrc->m_pPoint;
	CRoadPoint * pLastRecent = NULL;
	while (pRecent)
	{
		double fMoveDistance = CDoublePoint::GetDistance(*pPrev, *pRecent);
		double fTimeCost = fMoveDistance / fSpeed;
		hostRoute.InsertOneEntry(*pRecent, fTimeCost);
		if (pRecent == pDes)
		{
			break;
		}
		pLastRecent = pRecent;
		int nConnectedCount = pRecent->m_ConnectedPoints.GetSize();
		ASSERT(nConnectedCount == 2);
		for (int i = 0; i < nConnectedCount; ++i)
		{
			if (pRecent->m_ConnectedPoints[i] != pPrev)
			{
				pRecent = pRecent->m_ConnectedPoints[i];
			}
		}
		ASSERT(pLastRecent != pRecent);
		pPrev = pLastRecent;
	}
}

void CRoadNet::RouteTwoRoadPoints(CHost & host, CRoadPoint * pA, CRoadPoint * pB, CHostRouteTable & hostRoute)
{
	CHostRouteTable insideRoad;
	double fMin = RouteInsideRoad(host, pA, pB, insideRoad);
	bool bInsideRoad = true;
	hostRoute.Reset();
	CArray<CRoadRouteAgency> resultA, resultB;
	pA->GetRouteAgency(resultA, m_allInterSections);
	pB->GetRouteAgency(resultB, m_allInterSections);
	CRoadRouteAgency bestA, bestB;
	for (int i = 0; i < resultA.GetSize(); ++i)
	{
		for (int j = 0; j < resultB.GetSize(); ++j)
		{
			int nDijAId = resultA[i].m_pIntersection->GetDij()->m_nID;
			int nDijBId = resultB[j].m_pIntersection->GetDij()->m_nID;
			double fDistance = m_pDijTable->GetDistance(nDijAId, nDijBId);
			double fDistanceAll = fDistance + resultA[i].m_fDistance + resultB[j].m_fDistance;
			if (fDistanceAll < fMin)
			{
				fMin = fDistanceAll;
				bestA = resultA[i];
				bestB = resultB[j];
				bInsideRoad = false;
			}
		}
	}
	if (bInsideRoad == false)
	{
		RoutePointToAgency(host, pA, bestA, hostRoute);
		RouteAgencyToAgency(host, bestA, bestB, hostRoute);
		RouteAgencyToPoint(host, bestB, pB, hostRoute);
	}
	else
	{
		hostRoute = insideRoad;
	}
}

void CRoadNet::RoutePointToAgency(CHost & host, CRoadPoint * pA, CRoadRouteAgency & agencyB, CHostRouteTable & hostRoute)
{
	int nRecentId = pA->GetId();
	int nDesId = agencyB.m_pIntersection->m_pPoint->GetId();
	while (nRecentId != nDesId)
	{
		hostRoute.InsertOneEntryWithSpeed(*m_allPoints[nRecentId], host.GetDefaultSpeed());
		CRoadRouter * pRoadRouter = m_allPoints[nRecentId]->GetNextPointIdToIntersection(agencyB.m_pIntersection->GetId());
		nRecentId = pRoadRouter->m_nNextPointId;
	}
	hostRoute.InsertOneEntryWithSpeed(*m_allPoints[nRecentId], host.GetDefaultSpeed());
}

void CRoadNet::RouteAgencyToPoint(CHost & host, CRoadRouteAgency & agencyA, CRoadPoint * pB, CHostRouteTable & hostRoute)
{
	CRoadPoint * pStart = agencyA.m_pIntersection->m_pPoint;
	if (pStart->GetId() == pB->GetId())
	{
		return;
	}
	int nNextId = -1;
	for (int i = 0; i < pStart->m_InsideRouter.m_AllEntries.GetSize(); ++i)
	{
		if (pStart->m_InsideRouter.m_AllEntries[i].m_nDesId == pB->GetId())
		{
			nNextId = pStart->m_InsideRouter.m_AllEntries[i].m_nNextId;
			break;
		}
	}
	ASSERT(nNextId != -1);

	CRoadPoint * pPrev = pStart;
	CRoadPoint * pNext = m_allPoints[nNextId];
	int nChoiceCount = pNext->m_ConnectedPoints.GetSize();
	while (pPrev != pB)
	{
		hostRoute.InsertOneEntryWithSpeed(*pPrev, host.GetDefaultSpeed());

		if (pNext == pB)
		{
			break;
		}

		nChoiceCount = pNext->m_ConnectedPoints.GetSize();
		if (nChoiceCount != 2)
		{
			ASSERT(false);
			break;
		}
		if (pNext->m_ConnectedPoints[0]->GetId() == pPrev->GetId())
		{
			pPrev = pNext;
			pNext = pNext->m_ConnectedPoints[1];
		}
		else
		{
			ASSERT(pNext->m_ConnectedPoints[1]->GetId() == pPrev->GetId());
			pPrev = pNext;
			pNext = pNext->m_ConnectedPoints[0];
		}
	}
	hostRoute.InsertOneEntryWithSpeed(*pNext, host.GetDefaultSpeed());
}

void CRoadNet::RouteAgencyToAgency(CHost & host, CRoadRouteAgency & A, CRoadRouteAgency & B, CHostRouteTable & hostRoute)
{
	if (A.m_pIntersection == B.m_pIntersection)
	{
		return;
	}
	int nDijIdA = A.m_pIntersection->GetDij()->m_nID;
	int nDijIdB = B.m_pIntersection->GetDij()->m_nID;
	int nNextId = m_pDijTable->GetNextHocId(nDijIdA, nDijIdB);
	CIntersection * pPrevIntersection = A.m_pIntersection;
	CIntersection * pNextIntersection = m_allDijNodes[nNextId]->m_pIntersection;

	while (true)
	{
		CInterToInter tmpI2I = pPrevIntersection->GetInterToInter(pNextIntersection);
		CHostRouteEntry tmpEntry;
		double fDistanceInc = 0.0;
		for (int i = 0; i < tmpI2I.m_Points.GetSize(); ++i)
		{
			hostRoute.InsertOneEntryWithSpeed(*tmpI2I.m_Points[i], host.GetDefaultSpeed());
		}
		if (pNextIntersection == B.m_pIntersection)
		{
			return;
		}
		int nNextId = m_pDijTable->GetNextHocId(pNextIntersection->GetDij()->m_nID, nDijIdB);
		pPrevIntersection = pNextIntersection;
		pNextIntersection = m_allDijNodes[nNextId]->m_pIntersection;
	}
}

double CRoadNet::RouteInsideRoad(CHost & host, CRoadPoint * pA, CRoadPoint * pB, CHostRouteTable & hostRoute)
{
	double fRouteLength = 0.0;
	bool bFirstTime = true;
	CRoadPoint * pStart = pA;
	int nNextId = -1;
	for (int i = 0; i < pStart->m_InsideRouter.m_AllEntries.GetSize(); ++i)
	{
		if (pStart->m_InsideRouter.m_AllEntries[i].m_nDesId == pB->GetId())
		{
			nNextId = pStart->m_InsideRouter.m_AllEntries[i].m_nNextId;
			break;
		}
	}
	if (nNextId == -1)
	{
		return DBL_MAX;
	}

	CRoadPoint * pPrev = pStart;
	CRoadPoint * pNext = m_allPoints[nNextId];
	int nChoiceCount = pNext->m_ConnectedPoints.GetSize();
	while (pPrev != pB)
	{
		fRouteLength += hostRoute.InsertOneEntryWithSpeed(*pPrev, host.GetDefaultSpeed());
		if (bFirstTime)
		{
			ASSERT(fRouteLength < 0.0001);
			bFirstTime = false;
			fRouteLength = 0;
		}

		if (pNext == pB)
		{
			break;
		}

		nChoiceCount = pNext->m_ConnectedPoints.GetSize();
		if (nChoiceCount != 2)
		{
			ASSERT(false);
			break;
		}
		if (pNext->m_ConnectedPoints[0]->GetId() == pPrev->GetId())
		{
			pPrev = pNext;
			pNext = pNext->m_ConnectedPoints[1];
		}
		else
		{
			ASSERT(pNext->m_ConnectedPoints[1]->GetId() == pPrev->GetId());
			pPrev = pNext;
			pNext = pNext->m_ConnectedPoints[0];
		}
	}
	fRouteLength += hostRoute.InsertOneEntryWithSpeed(*pNext, host.GetDefaultSpeed());
	return fRouteLength;
}

void CRoadNet::ConnectBuildAndRoad(CBuilding * pBuilding, PtrRoadPoint pRoadPoint)
{
	pBuilding->AddRoadPoint(pRoadPoint);
}

void CRoadNet::SplitLongStraightRoads(double fLengthLimit, int nSplitLimit)
{
	for (int i = 0; i < m_allPoints.GetSize(); ++i)
	{
		for (int j = 0; j < m_allPoints[i]->m_ConnectedPoints.GetSize(); ++j)
		{
			PtrRoadPoint pRoadPointA = m_allPoints[i];
			PtrRoadPoint pRoadPointB = m_allPoints[i]->m_ConnectedPoints[j];
			SplitLongStraightRoad(pRoadPointA, pRoadPointB, fLengthLimit, nSplitLimit);
		}
	}
}

void CRoadNet::SplitLongStraightRoad(PtrRoadPoint pRoadPointA, PtrRoadPoint pRoadPointB, double fLengthLimit, int nSplitLimit)
{
	double fDistance = CDoublePoint::GetDistance(*pRoadPointA, *pRoadPointB);
	int nNewLineCount = fDistance / fLengthLimit + 1;
	if (nNewLineCount == 1)
	{
		return;
	}
	if (nNewLineCount > nSplitLimit)
	{
		nNewLineCount = nSplitLimit;
	}
	double fSpliteDistance = fDistance / nNewLineCount;
	int nRoadId = m_allPoints.GetSize();
	PtrRoadPoint pLeft = pRoadPointA;
	bool bRight = false;
	for (int i = 1; i < nNewLineCount; ++i)
	{
		double x = (i*fSpliteDistance / fDistance)*(pRoadPointB->m_X - pRoadPointA->m_X) + pRoadPointA->m_X;
		double y = (i*fSpliteDistance / fDistance)*(pRoadPointB->m_Y - pRoadPointA->m_Y) + pRoadPointA->m_Y;
		PtrRoadPoint pNew = new CRoadPoint(x, y);
		pNew->SetId(nRoadId++);
		AddNewRoadPoint(pNew);
		if (pLeft == pRoadPointA)
		{
			for (int j = 0; j < pRoadPointA->m_ConnectedPoints.GetSize(); ++j)
			{
				if (pRoadPointA->m_ConnectedPoints[j] == pRoadPointB)
				{
					pRoadPointA->m_ConnectedPoints[j] = pNew;
					pNew->InsertConnectedPoint(pRoadPointA);

					pLeft = pNew;
					bRight = true;
					break;
				}
			}
		}
		else
		{
			CRoadPoint::ConnectTwoPoints(*pLeft, *pNew);
			pLeft = pNew;
		}
	}
	ASSERT(bRight);
	bRight = false;
	for (int j = 0; j < pRoadPointB->m_ConnectedPoints.GetSize(); ++j)
	{
		if (pRoadPointB->m_ConnectedPoints[j] == pRoadPointA)
		{
			pRoadPointB->m_ConnectedPoints[j] = pLeft;
			pLeft->InsertConnectedPoint(pRoadPointB);
			bRight = true;
			break;
		}
	}
	ASSERT(bRight);
}
