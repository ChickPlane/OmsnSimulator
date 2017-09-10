#include "stdafx.h"
#include "RoadInsideRouterEntry.h"


CRoadInsideRouterEntry::CRoadInsideRouterEntry()
{
}


CRoadInsideRouterEntry::CRoadInsideRouterEntry(const CRoadInsideRouterEntry & src)
{
	*this = src;
}

CRoadInsideRouterEntry & CRoadInsideRouterEntry::operator=(const CRoadInsideRouterEntry & src)
{
	m_nSrcId = src.m_nSrcId;
	m_nNextId = src.m_nNextId;
	m_nDesId = src.m_nDesId;
	m_fDistance = src.m_fDistance;
	return *this;
}

CRoadInsideRouterEntry::~CRoadInsideRouterEntry()
{
}
