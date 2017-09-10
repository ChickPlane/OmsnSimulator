#include "stdafx.h"
#include "RoadRouter.h"


CRoadRouter::CRoadRouter()
{
}


CRoadRouter::~CRoadRouter()
{
}

void CRoadRouter::SetValue(int nCurrentPointId, int nIntersectionId, int nNextPointId, double fDistance)
{
	m_nCurrentPointId = nCurrentPointId;
	m_nIntersectionId = nIntersectionId;
	m_nNextPointId = nNextPointId;
	m_fDistance = fDistance;
}
