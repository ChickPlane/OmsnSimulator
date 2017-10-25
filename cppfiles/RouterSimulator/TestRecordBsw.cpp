#include "stdafx.h"
#include "TestRecordBsw.h"


CTestRecordBsw::CTestRecordBsw()
{
	for (int i = 0; i < REC_ST_MAX; ++i)
	{
		m_lnTimes[i] = -1;
	}
}


CTestRecordBsw::~CTestRecordBsw()
{
}
