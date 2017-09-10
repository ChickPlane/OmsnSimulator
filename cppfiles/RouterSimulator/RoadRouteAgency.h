#pragma once
class CRoadPoint;
class CIntersection;

class CRoadRouteAgency
{
public:
	CRoadRouteAgency();
	CRoadRouteAgency(const CRoadRouteAgency & src);
	CRoadRouteAgency & operator = (const CRoadRouteAgency & src);
	~CRoadRouteAgency();
	CRoadPoint * m_pRoadPoint;
	CIntersection * m_pIntersection;
	double m_fDistance;
};

