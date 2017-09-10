#pragma once
#include "streethead.h"
#include "RoadPointConnection.h"
#include "IntersectionRouter.h"
#include "InterToInter.h"

class CDijNode;

class CIntersection
{
public:
	CIntersection();
	CIntersection(PtrRoadPoint pPoint);
	~CIntersection();

	void InsertNearbyPoints(int nDestinationPointId, int nNextPointId, double fDistance);
	CIntersectionRouter * GetNextPointRouter(int nDesPointId);

	bool IsDijNode() const;
	CDijNode * SetDijState(int nID);
	CDijNode * GetDij();
	const CDijNode * GetDij() const;
	void SetId(int nId);
	int GetId();

	PtrRoadPoint m_pPoint;
	CArray<CIntersectionRouter> m_NearbyPoints;
	CArray<CInterToInter> m_InterToInterConnects;
	CInterToInter & GetInterToInter(CIntersection * pDes);

private:
	CDijNode * m_pDij;
	int m_nId;
};


