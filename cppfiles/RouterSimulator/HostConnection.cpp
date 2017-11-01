#include "stdafx.h"
#include "HostConnection.h"

CHostConnectionEntry::CHostConnectionEntry()
	: m_pNeighbor(NULL)
	, m_lnLastUpdate(0)
	, m_nUpdateTimes(0)
{

}


CHostConnectionEntry::CHostConnectionEntry(const CHostConnectionEntry & src)
{
	*this = src;
}

CHostConnectionEntry & CHostConnectionEntry::operator=(const CHostConnectionEntry & src)
{
	m_pNeighbor = src.m_pNeighbor;
	m_lnLastUpdate = src.m_lnLastUpdate;
	m_nUpdateTimes = src.m_nUpdateTimes;
	return *this;
}

CHostConnection::CHostConnection()
	:m_lnLastUpdate(0)
{
}


CHostConnection::~CHostConnection()
{
}

void CHostConnection::Reset(int nRecordCount)
{
	m_Records.RemoveAll();
	m_Records.SetSize(nRecordCount);
}

int CHostConnection::UpdateSingleHost(CHost * pHost, const CReceiverReportItem & item, SIM_TIME lnUpdateTime)
{
	int nItemHostCount = item.m_Hosts.GetSize();
	int nHostId = pHost->m_nId;
	CMap<int, int, CHostConnectionEntry, CHostConnectionEntry&> & modifyingRecord = m_Records[nHostId];
	POSITION pos = item.m_Hosts.GetHeadPosition();
	while (pos)
	{
		const CHostGui& tmpNeighbor = item.m_Hosts.GetNext(pos);
		int nKey = tmpNeighbor.m_pHost->m_nId;
		CHostConnectionEntry connectionEntry;
		if (modifyingRecord.Lookup(nKey, connectionEntry))
		{
			ASSERT(connectionEntry.m_lnLastUpdate < lnUpdateTime);
			ASSERT(connectionEntry.m_pNeighbor == tmpNeighbor.m_pHost);
			connectionEntry.m_nUpdateTimes++;
		}
		else
		{
			connectionEntry.m_nUpdateTimes = 1;
			ASSERT(tmpNeighbor.m_pHost != NULL);
			connectionEntry.m_pNeighbor = tmpNeighbor.m_pHost;
		}
		connectionEntry.m_lnLastUpdate = lnUpdateTime;
		modifyingRecord[nKey] = connectionEntry;
	}

	int rKey = 0;
	CHostConnectionEntry rValue;

	CList<int> toBeDeleteList;
	pos = modifyingRecord.GetStartPosition();
	while (pos)
	{
		modifyingRecord.GetNextAssoc(pos, rKey, rValue);
		if (rValue.m_lnLastUpdate != lnUpdateTime)
		{
			toBeDeleteList.AddTail(rKey);
		}
	}

	pos = toBeDeleteList.GetHeadPosition();
	while (pos)
	{
		modifyingRecord.RemoveKey(toBeDeleteList.GetNext(pos));
	}

	int nModifyLength = modifyingRecord.GetSize();
	ASSERT(nModifyLength >= 1);
	return nModifyLength;
}
