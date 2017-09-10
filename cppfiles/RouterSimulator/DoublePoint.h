#pragma once
#include <cmath>
class CDoublePoint
{
public:

	CDoublePoint()
		:m_X(0),m_Y(0)
	{
	}

	CDoublePoint(double x, double y)
	{
		SetValue(x, y);
	}

	~CDoublePoint()
	{
	}

	void SetValue(double x, double y)
	{
		m_X = x;
		m_Y = y;
	}

	bool operator == (const CDoublePoint & src) const
	{
		if (abs(src.m_X - m_X) > 0.0001)
		{
			return false;
		}
		if (abs(src.m_Y - m_Y) > 0.0001)
		{
			return false;
		}
		return true;
	}

	bool operator != (const CDoublePoint & src) const
	{
		return !(*this == src);
	}

	bool CompareCoordinate(const CDoublePoint * src) const
	{
		return *this == *src;
	}

	static double GetDistance(const CDoublePoint & a, const CDoublePoint & b)
	{
		double fX = a.m_X - b.m_X;
		fX = fX*fX;
		double fY = a.m_Y - b.m_Y;
		fY = fY*fY;
		double fSum = (fX + fY);
		return sqrt(fSum);
	}

	double m_X;
	double m_Y;
};

