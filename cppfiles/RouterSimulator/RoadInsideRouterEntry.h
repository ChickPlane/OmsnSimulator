#pragma once
class CRoadInsideRouterEntry
{
public:
	CRoadInsideRouterEntry();
	CRoadInsideRouterEntry(const CRoadInsideRouterEntry & src);
	CRoadInsideRouterEntry & operator = (const CRoadInsideRouterEntry & src);
	~CRoadInsideRouterEntry();
	int m_nSrcId;
	int m_nNextId;
	int m_nDesId;
	double m_fDistance;
};

