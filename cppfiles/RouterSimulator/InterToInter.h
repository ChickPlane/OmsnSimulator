#pragma once
#include "streethead.h"
class CInterToInter
{
public:
	CInterToInter();
	CInterToInter(const CInterToInter & src);
	CInterToInter & operator = (const CInterToInter & src);
	~CInterToInter();
	CArray<PtrRoadPoint> m_Points;
	double m_fDistance;
};

