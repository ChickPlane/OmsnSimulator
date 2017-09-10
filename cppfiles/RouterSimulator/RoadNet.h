#pragma once
#include "streethead.h"
#include "LineInMap.h"
#include "DijTable.h"
#include "PointKey.h"
#include "PointReference.h"
#include "Building.h"
#include "HostRouteTable.h"
#include "InterToInter.h"
#include "Host.h"
#include "RoadRouteAgency.h"

class CHostEngine;

class CRoadNet
{
public:
	CRoadNet();
	~CRoadNet();
	CArray<PtrRoadPoint> m_allPoints;
	CMap<int, int, CPointReference *, CPointReference *> m_Reference;

	CArray<PtrIntersection> m_allInterSections;
	CArray<PtrDijNode> m_allDijNodes;
	DijTable * m_pDijTable;

	CArray<CBuilding *> m_allBuildings;

	CArray<CHost *> m_allHosts;

	bool HasHash(const CDoublePoint & testPoint) const;

	void Init(const CList<CLineInMap> & lines, double & l, double & r, double & t, double b);
	void AddOneRoad(const CList<PtrRoadPoint> & newLine);
	void DetermineAllInterSections();
	void DetermineAllRoadInsideRoute();
	void DetermineAllDij();
	void SetAllDijDistance();
	void InitOriginalDistances();
	void InitOriginalFromIntersectionStart(int nIntersectionId);
	void InitOriginalFromIntersection(int nIntersectionId, PtrRoadPoint pIntersectionNext, PtrRoadPoint pDesPrev, PtrRoadPoint pDes, double fDistance);
	DWORD GetHashValue(const CDoublePoint * pPoint);
	DWORD GetHashValue(double fX, double fY);
	double GetHashInterval();
	SIM_TIME GetSimTimeCrossHalfBlank();

	void GetMapRange(CDoublePoint & lefttop, CDoublePoint & rightbottom);
	void GetMapCenter(CDoublePoint & center);
	void AddNewRoadPoint(PtrRoadPoint pRoadPoint);
	PtrRoadPoint FindRoadPoint(const CDoublePoint & coor);
	PtrRoadPoint FindNearestRoadPoints(const CDoublePoint & coor, double fDistance);
	PtrRoadPoint FindNearestRoadPoint(const CDoublePoint & coor);
	int GetHostNumberInRange(const CDoublePoint & coor, double fRadius, SIM_TIME lnTime) const;
	double GetSpeedLimit();

	void LoadBuildingSettings(char * strFileName);
	void ConnectBuildingsToRoads();
	void BuildAllInterToInter();
	void BuildInterToInter(CIntersection * pSrc);
	void GetAllPointsBetweenIntersections(CInterToInter & result, CIntersection * pSrc, CIntersection * pDes);
	void WalkAlongRoad(CHostRouteTable & hostRoute, CIntersection * pSrc, CRoadPoint * pNext, CRoadPoint * pDes, double fSpeed);
	void RouteTwoRoadPoints(CHost & host, CRoadPoint * pA, CRoadPoint * pB, CHostRouteTable & hostRoute);
	void RoutePointToAgency(CHost & host, CRoadPoint * pA, CRoadRouteAgency & agencyB, CHostRouteTable & hostRoute);
	void RouteAgencyToPoint(CHost & host, CRoadRouteAgency & agencyA, CRoadPoint * pB, CHostRouteTable & hostRoute);
	void RouteAgencyToAgency(CHost & host, CRoadRouteAgency & A, CRoadRouteAgency & B, CHostRouteTable & hostRoute);
	double RouteInsideRoad(CHost & host, CRoadPoint * pA, CRoadPoint * pB, CHostRouteTable & hostRoute);
protected:
	void ConnectBuildAndRoad(CBuilding * pBuilding, PtrRoadPoint pRoadPoint);
	void SplitLongStraightRoads(double fLengthLimit, int nSplitLimit);
	void SplitLongStraightRoad(PtrRoadPoint pRoadPointA, PtrRoadPoint pRoadPointB, double fLengthLimit, int nSplitLimit);

private:
	CList<PtrDijNode> m_allTmpDijNodes;
	CDoublePoint m_lefttop;
	CDoublePoint m_rightbottom;
	CPointKey m_PointHash;
};

