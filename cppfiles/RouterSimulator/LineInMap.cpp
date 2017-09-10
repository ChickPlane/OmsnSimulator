#include "stdafx.h"
#include "LineInMap.h"


CLineInMap::CLineInMap()
{
}


CLineInMap::CLineInMap(const CLineInMap & src)
{
	*this = src;
}

CLineInMap::~CLineInMap()
{
}

CLineInMap & CLineInMap::operator=(const CLineInMap & src)
{
	m_Line.Copy(src.m_Line);
	return *this;
}
