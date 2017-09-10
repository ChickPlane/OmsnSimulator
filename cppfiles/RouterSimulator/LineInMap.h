#pragma once
#include "DoublePoint.h"
class CLineInMap
{
public:
	CLineInMap();
	CLineInMap(const CLineInMap & src);
	~CLineInMap();

	CLineInMap & operator = (const CLineInMap & src);
	CArray<CDoublePoint> m_Line;
};

