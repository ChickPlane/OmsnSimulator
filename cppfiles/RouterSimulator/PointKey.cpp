#include "stdafx.h"
#include "PointKey.h"
#include "DoublePoint.h"



CPointKey::CPointKey()
{
}


CPointKey::~CPointKey()
{
}

DWORD CPointKey::GetHashValue(const CDoublePoint * pPoint)
{
	return GetHashValue(pPoint->m_X, pPoint->m_Y);
}

DWORD CPointKey::GetHashValue(double fX, double fY)
{
	unsigned int nX = (fX - m_lefttop.m_X) / m_fInterval + 0.00000000005;
	unsigned int nY = (fY - m_lefttop.m_Y) / m_fInterval + 0.00000000005;
#if 0
	DWORD ret = nX ^ nY;
	ret |= ret << 16;
	ret ^= ret << 8;
#else
	DWORD ret = (nY << 18) | (nX << 4);
	//ret ^= (ret << 7);
#endif
	return ret;
}

int CPointKey::SetValue(const CDoublePoint & lefttop, const CDoublePoint & rightbottom, double fInterval)
{
	unsigned short usAxisNumber = -1;
	double fMinXInterval = (rightbottom.m_X - lefttop.m_X) / usAxisNumber;
	double fMinYInterval = (rightbottom.m_Y - lefttop.m_Y) / usAxisNumber;
	double fMinInterval = fMinXInterval < fMinYInterval ? fMinYInterval : fMinXInterval;
	if (fInterval < fMinInterval)
	{
		ASSERT(false);
		fInterval = fMinInterval;
	}
	m_lefttop = lefttop;
	m_rightbottom = rightbottom;
	m_fInterval = fInterval;
	return m_fInterval;
}
