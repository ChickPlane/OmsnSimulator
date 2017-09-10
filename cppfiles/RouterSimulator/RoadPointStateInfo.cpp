#include "stdafx.h"
#include "RoadPointStateInfo.h"
#include "Intersection.h"


CRoadPointStateInfo::CRoadPointStateInfo()
{
	Reset();
}


CRoadPointStateInfo::~CRoadPointStateInfo()
{
}

COLORREF CRoadPointStateInfo::GetColor()
{
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 0;
	if (bMarkedByDoubleClick)
	{
		b += 100;
		return RGB(0, 0, 200);
	}
	if (bIsDijAlgorithmFinished)
	{
		return RGB(0, 255, 50);
	}
	return MARKCOLOR_NULL;
	if (bIsIntersection)
	{
		r += 100;
	}
	if (bIsDijPoint)
	{
		return RGB(0, 255, 0);
		r += 100;
		g += 100;
	}
	return RGB(r,g,b);
}

void CRoadPointStateInfo::Reset()
{
	bMarkedByDoubleClick = false;
	bIsIntersection = false;
	bIsDijPoint = false;
	bIsDijAlgorithmFinished = false;
}

void CRoadPointStateInfo::SetValue(const CRoadPoint & roadpoint)
{
	bIsIntersection = false;
	bIsDijPoint = false;
	if (roadpoint.IsIntersection())
	{
		bIsIntersection = true;

		if (roadpoint.GetIntersection()->IsDijNode())
		{
			bIsDijPoint = true;
		}
	}
}
