#include "stdafx.h"
#include "IntersectionRouter.h"


CIntersectionRouter::CIntersectionRouter()
{
}


CIntersectionRouter::~CIntersectionRouter()
{
}

void CIntersectionRouter::SetValue(int nCurrentInterId, int nDestinationPointId, int nNextPointId, double fDistance)
{
	m_nCurrentInterId = nCurrentInterId;
	m_nDestinationPointId = nDestinationPointId;
	m_nNextPointId = nNextPointId;
	m_fDistance = fDistance;
}
