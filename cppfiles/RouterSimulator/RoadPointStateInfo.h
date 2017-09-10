#pragma once
#include "RoadPoint.h"

#define MARKCOLOR_NULL RGB(0,0,0)

class CRoadPointStateInfo
{
public:
	CRoadPointStateInfo();
	virtual ~CRoadPointStateInfo();
	COLORREF GetColor();

	void Reset();

	void SetValue(const CRoadPoint & roadpoint);

	bool bMarkedByDoubleClick;
	bool bIsIntersection;
	bool bIsDijPoint;
	bool bIsDijAlgorithmFinished;
};

