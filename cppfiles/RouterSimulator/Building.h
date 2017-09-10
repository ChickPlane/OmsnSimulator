#pragma once
#include "DoublePoint.h"
#include "streethead.h"
class CBuilding
	: public CDoublePoint
{
public:
	CBuilding();
	CBuilding(const CBuilding & src);

	CBuilding(double x, double y);

	CBuilding & operator = (const CBuilding & src);
	virtual ~CBuilding();
	CString m_strMainType;
	CString m_strSubType;

	bool IsApartment();
	void AddRoadPoint(PtrRoadPoint pRoadPoint);
	
	CArray<PtrRoadPoint> m_pRoadPoints;
	int m_nId;
};

