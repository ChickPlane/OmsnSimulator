#pragma once
#include "streethead.h"
#include "DoublePoint.h"
class CPointReference
{
public:
	CPointReference();
	CPointReference(const CPointReference & src);
	CPointReference & operator = (const CPointReference & src);
	void AddNewPoint(CDoublePoint * pPoint);
	CDoublePoint * FindPointCoor(const CDoublePoint & cmp);
	int FindInRange(const CDoublePoint & cmp, double fDistance, CArray<CDoublePoint *> & result);
	CDoublePoint * FindNearest(const CDoublePoint & cmp);
	virtual ~CPointReference();
	CArray<CDoublePoint*> m_Points;
};

