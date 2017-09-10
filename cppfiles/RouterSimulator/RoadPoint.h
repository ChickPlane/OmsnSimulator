#pragma once
#include "DoublePoint.h"
#include "streethead.h"
#include "RoadRouter.h"
#include "RoadInsideRouter.h"
#include "RoadRouteAgency.h"


class CRoadPoint :
	public CDoublePoint
{
public:
	CRoadPoint();
	CRoadPoint(double x, double y);
	CRoadPoint(const CDoublePoint & src);
	~CRoadPoint();
	void InitValues();

	CArray<PtrRoadPoint> m_ConnectedPoints;
	void InsertConnectedPoint(PtrRoadPoint pConnected);
	static void InsertConnectedPoints(const CList<PtrRoadPoint> & newLine, POSITION pos);
	static void ConnectTwoPoints(CRoadPoint & pointA, CRoadPoint & pointB);
	void InsertNearbyIntersections(int nIntersectionId, int nNextPointId, double fDistance);
	void SetId(int nId);
	int GetId();

	PtrIntersection InitIntersectionState();
	PtrIntersection SetAsIntersection(bool bSet);
	PtrIntersection GetIntersection();
	const PtrIntersection GetIntersection() const;
	bool IsIntersection() const;
	CRoadRouter * GetNextPointIdToIntersection(int nIntersectionId);
	CArray<CRoadRouter> m_NearbyIntersections;
	CRoadInsideRouter m_InsideRouter;
	void GetRouteAgency(CArray<CRoadRouteAgency> & result, CArray<PtrIntersection> & m_allInterSections);
private:
	CIntersection * m_pIntersection;
	int m_nId;
};

