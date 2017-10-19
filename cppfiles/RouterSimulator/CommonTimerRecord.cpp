#include "stdafx.h"
#include "CommonTimerRecord.h"


CCommonTimerRecord::CCommonTimerRecord()
{
}


CCommonTimerRecord::CCommonTimerRecord(const CCommonTimerRecord & src)
{
	*this = src;
}

CCommonTimerRecord::~CCommonTimerRecord()
{
}

CCommonTimerRecord & CCommonTimerRecord::operator=(const CCommonTimerRecord & src)
{
	m_nIDEvent = src.m_nIDEvent;
	m_pCommonTimer = src.m_pCommonTimer;
	m_bAlways = src.m_bAlways;
	return *this;
}
