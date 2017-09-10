#include "stdafx.h"
#include "DijItem.h"
#include "DijNode.h"
#include "DijConnection.h"
#include "Intersection.h"



CDijItem::CDijItem()
	: m_nFrom(-1)
	, m_nTo(-1)
	, m_nNext(-1)
	, m_fDistance(DBL_MAX)
	, m_bFixed(false)
{
}


CDijItem::~CDijItem()
{
}

void CDijItem::SetValue(const CDijConnection & dijConnect)
{
	m_nFrom = dijConnect.GetFirstDijNode()->m_nID;
	m_nTo = dijConnect.GetLastDijNode()->m_nID;
	m_nNext = m_nTo;
	m_fDistance = dijConnect.m_fDistance;
}

void CDijItem::SetValue(int nFrom, int nTo, int nNext, double fDistance)
{
	if (m_bFixed != false)
	{
		ASSERT(m_nFrom == nFrom);
		ASSERT(m_nTo == nTo);
		ASSERT(m_nNext == nNext);
		ASSERT(fabs(m_fDistance - fDistance) < DOUBLE_COMPARE_LIMIT);
		return;
	}
	else
	{
#if 0
		CString strOut;
		strOut.Format(_T("\n[%d]->[%d]:[%d] [%d]->[%d]:[%d] %e"), m_nFrom, m_nTo, m_nNext, nFrom, nTo, nNext, fabs(m_fDistance - fDistance));
		OutputDebugString(strOut);
#endif
		m_nFrom = nFrom;
		m_nTo = nTo;
		m_nNext = nNext;
		m_fDistance = fDistance;
	}
}

void CDijItem::SetFixed()
{
	m_bFixed = true;
}

bool CDijItem::IsFixed()
{
	return m_bFixed;
}

bool CDijItem::operator!=(const CDijItem & src) const
{
	return !operator==(src);
}

bool CDijItem::operator==(const CDijItem & src) const
{
	if (m_nFrom != src.m_nFrom)
	{
		return false;
	}
	if (m_nTo != src.m_nTo)
	{
		return false;
	}
	if (m_nNext != src.m_nNext)
	{
		return false;
	}
	if (fabs(m_fDistance - src.m_fDistance) > DOUBLE_COMPARE_LIMIT)
	{
		return false;
	}
	if (m_bFixed != src.m_bFixed)
	{
		return false;
	}
	return true;
}
