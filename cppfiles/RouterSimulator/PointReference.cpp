#include "stdafx.h"
#include "PointReference.h"
#include "DoublePoint.h"


CPointReference::CPointReference()
{
}


CPointReference::CPointReference(const CPointReference & src)
{
	*this = src;
}

CPointReference & CPointReference::operator=(const CPointReference & src)
{
	m_Points.Copy(src.m_Points);
	return *this;
}

void CPointReference::AddNewPoint(CDoublePoint * pPoint)
{
	m_Points.Add(pPoint);
}

CDoublePoint * CPointReference::FindPointCoor(const CDoublePoint & cmp)
{
	int nCount = m_Points.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		if (cmp.CompareCoordinate((CDoublePoint*)m_Points[i]))
		{
			return m_Points[i];
		}
	}
	return NULL;
}

int CPointReference::FindInRange(const CDoublePoint & cmp, double fDistance, CArray<CDoublePoint *> & result)
{
	result.RemoveAll();

	int nCount = m_Points.GetSize();
	int nFindCount = 0;
	for (int i = 0; i < nCount; ++i)
	{
		if (CDoublePoint::GetDistance(cmp, *(CDoublePoint*)m_Points[i]) <= fDistance)
		{
			++nFindCount;
			result.Add(m_Points[i]);
		}
	}
	return nFindCount;
}

CDoublePoint * CPointReference::FindNearest(const CDoublePoint & cmp)
{
	double fMin = DBL_MAX;
	CDoublePoint * result = NULL;
	int nCount = m_Points.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		double fDistance = CDoublePoint::GetDistance(cmp, *(CDoublePoint*)m_Points[i]);
		if (fDistance < fMin)
		{
			fMin = fDistance;
			result = m_Points[i];
		}
	}
	return result;
}

CPointReference::~CPointReference()
{
}
