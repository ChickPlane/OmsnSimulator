#include "stdafx.h"
#include "RoadRouteAgency.h"


CRoadRouteAgency::CRoadRouteAgency()
{
}


CRoadRouteAgency::CRoadRouteAgency(const CRoadRouteAgency & src)
{
	*this = src;
}

CRoadRouteAgency & CRoadRouteAgency::operator=(const CRoadRouteAgency & src)
{
	m_pRoadPoint = src.m_pRoadPoint;
	m_pIntersection = src.m_pIntersection;
	m_fDistance = src.m_fDistance;
	return *this;
}

CRoadRouteAgency::~CRoadRouteAgency()
{
}
