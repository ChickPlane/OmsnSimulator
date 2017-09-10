#include "stdafx.h"
#include "RoadPointConnection.h"
#include "streethead.h"
#include "Road.h"


CRoadPointConnection::CRoadPointConnection()
	: m_pSelf(NULL)
	, m_pOther(NULL)
	, m_nPosSelf(-1)
	, m_nPosOther(-1)
	, m_pRoad(NULL)
	, m_fDistance(DBL_MAX)
{
}

CRoadPointConnection::CRoadPointConnection(PtrRoad pRoad, int nSelf, int nOther, double fDistance)
{
	SetValue(pRoad, nSelf, nOther, fDistance);
}

CRoadPointConnection::CRoadPointConnection(const CRoadPointConnection & src)
{
	*this = src;
}

void CRoadPointConnection::SetValue(PtrRoad pRoad, int nSelf, int nOther, double fDistance)
{
	m_nPosSelf = nSelf;
	m_nPosOther = nOther;
	m_pSelf = pRoad->m_Points[nSelf];
	m_pOther = pRoad->m_Points[nOther];
	m_fDistance = fDistance;
}

CRoadPointConnection::~CRoadPointConnection()
{
}


bool CRoadPointConnection::SamePair(const PtrRoadPoint pSelf, const PtrRoadPoint pOther)
{
	if (m_pSelf == pSelf && m_pOther == pOther)
	{
		return true;
	}
	else
	{
		return false;
	}
}

PtrRoadPoint CRoadPointConnection::GetTheOtherEnd()
{
	return m_pOther;
}

PtrRoadPoint CRoadPointConnection::GetSelfEnd()
{
	return m_pSelf;
}

bool CRoadPointConnection::operator==(const CRoadPointConnection & src)
{
	if (SamePair(src.m_pSelf, src.m_pOther) == false)
	{
		return false;
	}
	if (m_fDistance != src.m_fDistance)
	{
		return false;
	}
	return true;
}

CRoadPointConnection & CRoadPointConnection::operator=(const CRoadPointConnection & src)
{
	m_pSelf = src.m_pSelf;
	m_pOther = src.m_pOther;
	m_fDistance = src.m_fDistance;
	return *this;
}
