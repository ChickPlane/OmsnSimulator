#include "stdafx.h"
#include "InterToInter.h"


CInterToInter::CInterToInter()
	: m_fDistance(DBL_MAX)
{
}


CInterToInter::CInterToInter(const CInterToInter & src)
{
	*this = src;
}

CInterToInter & CInterToInter::operator=(const CInterToInter & src)
{
	m_Points.Copy(src.m_Points);
	m_fDistance = src.m_fDistance;
	return *this;
}

CInterToInter::~CInterToInter()
{
}
