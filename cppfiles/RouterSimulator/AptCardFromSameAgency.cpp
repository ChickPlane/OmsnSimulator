#include "stdafx.h"
#include "AptCardFromSameAgency.h"


CAptCardFromSameAgency::CAptCardFromSameAgency()
{
}

CAptCardFromSameAgency::CAptCardFromSameAgency(const CAptCardFromSameAgency & src)
{
	*this = src;
}


CAptCardFromSameAgency & CAptCardFromSameAgency::operator=(const CAptCardFromSameAgency & src)
{
	m_Records.RemoveAll();
	POSITION pos = src.m_Records.GetHeadPosition();
	while (pos)
	{
		m_Records.AddTail(src.m_Records.GetNext(pos));
	}
	return *this;
}

CAptCardFromSameAgency::~CAptCardFromSameAgency()
{
}

CString CAptCardFromSameAgency::GetString() const
{
	CString strOut;
	POSITION pos = m_Records.GetHeadPosition();
	while (pos)
	{
		strOut += _T("\n") + m_Records.GetNext(pos).GetString();
	}
	return strOut;
}
