#pragma once
#include "Host.h"
#include "MsgCntJudgeReceiverReport.h"

class CHostConnectionEntry
{
public:
	CHostConnectionEntry();
	CHostConnectionEntry(const CHostConnectionEntry & src);
	CHostConnectionEntry & operator = (const CHostConnectionEntry & src);
	CHost * m_pNeighbor;
	SIM_TIME m_lnLastUpdate;
	int m_nUpdateTimes;
};

class CHostConnection
{
public:
	CHostConnection();
	virtual ~CHostConnection();
	void Reset(int nRecordCount);
	CArray<CMap<int, int, CHostConnectionEntry, CHostConnectionEntry&>> m_Records;

private:
	SIM_TIME m_lnLastUpdate;
};

