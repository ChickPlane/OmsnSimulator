#pragma once
#include "RoadInsideRouterEntry.h"
class CRoadInsideRouter
{
public:
	CRoadInsideRouter();
	~CRoadInsideRouter();
	void InsertItem(int nSrcId, int nNextId, int nDesId, double fDistance);
	CArray<CRoadInsideRouterEntry> m_AllEntries;
};