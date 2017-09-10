#pragma once
#include "streethead.h"
class CRoad
{
public:
	CRoad();
	~CRoad();
	void Init(const CList<PtrRoadPoint> & newLine);
	CArray<PtrRoadPoint> m_Points;

	void ReversePoints();
	void ReceivePointsFrom(CRoad * pNewRoad);
	int GetNeighbours(PtrRoadPoint pTarget, CList<PtrRoadPoint> & result);
};

