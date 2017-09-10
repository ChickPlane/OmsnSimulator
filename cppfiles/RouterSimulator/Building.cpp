#include "stdafx.h"
#include "Building.h"


CBuilding::CBuilding()
	: m_nId(-1)
{
}


CBuilding::CBuilding(double x, double y)
	: CDoublePoint(x, y)
	, m_nId(-1)
{

}

CBuilding::CBuilding(const CBuilding & src)
{
	*this = src;
}

CBuilding & CBuilding::operator=(const CBuilding & src)
{
	CDoublePoint::operator =(src);
	m_strMainType = src.m_strMainType;
	m_strSubType = src.m_strSubType;
	m_pRoadPoints.Copy(src.m_pRoadPoints);
	m_nId = src.m_nId;
	return *this;
}

CBuilding::~CBuilding()
{
}

bool CBuilding::IsApartment()
{
	if (m_strMainType == _T("building"))
	{
		if (m_strSubType == _T("house"))
		{
			return true;
		}
		if (m_strSubType == _T("yes"))
		{
			return true;
		}
		if (m_strSubType == _T("apartments"))
		{
			return true;
		}
		if (m_strSubType == _T("detached"))
		{
			return true;
		}
	}
}

void CBuilding::AddRoadPoint(PtrRoadPoint pRoadPoint)
{
	for (int i = 0; i < m_pRoadPoints.GetSize(); ++i)
	{
		if (m_pRoadPoints[i] == pRoadPoint)
		{
			return;
		}
	}
	m_pRoadPoints.Add(pRoadPoint);
}
