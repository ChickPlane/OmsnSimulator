#include "stdafx.h"
#include "TestRecordSlpd.h"


CTestRecordSlpd::CTestRecordSlpd()
{
	for (int i = 0; i < REC_SLPD_ST_MAX; ++i)
	{
		m_lnTimes[i] = -1;
	}
	m_nForwardTimes = 0;
}


CTestRecordSlpd::~CTestRecordSlpd()
{
}
