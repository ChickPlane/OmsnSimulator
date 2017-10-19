#pragma once
#include "streethead.h"
#include "DoublePoint.h"
class CPointKey
{
public:
	CPointKey();
	~CPointKey();
	DWORD GetHashValue(const CDoublePoint * pPoint);
	DWORD GetHashValue(double fX, double fY);
	int SetValue(const CDoublePoint & lefttop, const CDoublePoint & rightbottom, double fInterval);
	double GetInterval() const { return m_fInterval; }
private:
	CDoublePoint m_lefttop;
	CDoublePoint m_rightbottom;
	double m_fInterval;

};
