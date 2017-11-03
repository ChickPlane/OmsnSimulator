#include "stdafx.h"
#include "MsgCntJudgeReceiverReport.h"
#include "Host.h"
#include "CommonFunctions.h"


CReceiverReportItem::CReceiverReportItem()
	: m_bUnicast(FALSE)
	, m_pDirNeighbours(NULL)
	, m_nDirNeighbourCount(0)
	, m_pAllNeighbours(NULL)
	, m_nNeighbourCount(0)
{

}


CReceiverReportItem::CReceiverReportItem(const CReceiverReportItem & src)
{
	*this = src;
}

CReceiverReportItem::~CReceiverReportItem()
{
	if (m_pDirNeighbours)
	{
		delete[] m_pDirNeighbours;
		m_pDirNeighbours = NULL;
	}
	if (m_pAllNeighbours)
	{
		delete[] m_pAllNeighbours;
		m_pAllNeighbours = NULL;
	}
}

void CReceiverReportItem::GenerateDirNeighbourArr()
{
	m_nDirNeighbourCount = m_Hosts.GetSize() - 1;
	if (m_nDirNeighbourCount == 0)
	{
		return;
	}
	int nIndex = 0;
	m_pDirNeighbours = new int[m_nDirNeighbourCount];
	POSITION pos = m_Hosts.GetHeadPosition();
	while (pos)
	{
		int nId = m_Hosts.GetNext(pos).m_HopFrom.m_pHost->m_nId;
		if (nId == m_pCenterHost->m_nId)
		{
			continue;
		}
		m_pDirNeighbours[nIndex++] = nId;
	}
	ASSERT(nIndex == m_nDirNeighbourCount);
	CCommonFunctions_T<int>::QuickIncreaseSort(m_pDirNeighbours, nIndex);
}

void CReceiverReportItem::GenerateAllNeighbourArr()
{
	int nIndex = 0;
	m_pAllNeighbours = new int[m_nNeighbourCount];
	int rKey;
	POSITION rValue;

	POSITION pos = m_HopDestinations.GetStartPosition();
	while (pos)
	{
		m_HopDestinations.GetNextAssoc(pos, rKey, rValue);
		m_pAllNeighbours[nIndex++] = rKey;
	}
	ASSERT(m_nNeighbourCount == nIndex);
	CCommonFunctions_T<int>::QuickIncreaseSort(m_pAllNeighbours, nIndex);
}

CHost * CReceiverReportItem::GetNextHop(USERID nDestinationId) const
{
	POSITION posFrom = NULL;
	if (m_HopDestinations.Lookup(nDestinationId, posFrom))
	{
		return m_Hosts.GetAt(posFrom).m_HopFrom.m_pHost;
	}
	else
	{
		return NULL;
	}
}

BOOL CReceiverReportItem::IsAnyOneNearby() const
{
	return m_nDirNeighbourCount > 0;
}

CReceiverReportItem & CReceiverReportItem::operator=(const CReceiverReportItem & src)
{
	ASSERT(0);
	m_bUnicast = src.m_bUnicast;
	m_pCenterHost = src.m_pCenterHost;
	m_CenterLocation = src.m_CenterLocation;
	POSITION pos = src.m_Hosts.GetHeadPosition();
	while (pos)
	{
		m_Hosts.AddTail(src.m_Hosts.GetNext(pos));
	}
	return *this;
}

CMsgCntJudgeReceiverReport::CMsgCntJudgeReceiverReport()
	: m_bFullReport(FALSE)
	, m_bDijEnable(FALSE)
	, m_bHasMultiNeighbours(FALSE)
{
}


CMsgCntJudgeReceiverReport::CMsgCntJudgeReceiverReport(const CMsgCntJudgeReceiverReport & src)
{
	*this = src;
}

CMsgCntJudgeReceiverReport & CMsgCntJudgeReceiverReport::operator=(const CMsgCntJudgeReceiverReport & src)
{
	ASSERT(0);
	m_bFullReport = src.m_bFullReport;
	m_lnTime = src.m_lnTime;
	m_bDijEnable = src.m_bDijEnable;

	int nSize = src.m_ArrItems.GetSize();
	m_ArrItems.SetSize(nSize);
	for (int i = 0; i < nSize; ++i)
	{
		m_ArrItems[i] = src.m_ArrItems[i];
	}
	return *this;
}

CMsgCntJudgeReceiverReport::~CMsgCntJudgeReceiverReport()
{
}

void CMsgCntJudgeReceiverReport::RunDij()
{
	if (m_bDijEnable == TRUE)
	{
		return;
	}
	m_bDijEnable = TRUE;
	for (int i = 0; i < m_ArrItems.GetSize(); ++i)
	{
		RunDijFor(i);
	}
#ifdef DEBUG
// 	if (m_bHasMultiNeighbours)
// 	{
// 		ShowAllDij();
// 	}
#endif
}

void CMsgCntJudgeReceiverReport::ShowAllDij()
{
	if (!m_bDijEnable)
	{
		return;
	}
	for (int i = 0; i < m_ArrItems.GetSize(); ++i)
	{
		ShowAllDijFor(i);
	}
}

void CMsgCntJudgeReceiverReport::RunDijFor(int nIndex)
{
	CMap<CReceiverReportItem *, CReceiverReportItem *, CJudgeDijPair, CJudgeDijPair> detectedHosts;
	CJudgeDijPair tmpPair;
	tmpPair.m_pItem = &m_ArrItems[nIndex];
	tmpPair.m_Group = NULL;
	detectedHosts[tmpPair.m_pItem] = tmpPair;

	CList<CJudgeDijPair> searchingHosts;

	int nDirectNeighbourCount = 0;
	int nAllNeighbourCount = 0;

	POSITION pos = m_ArrItems[nIndex].m_Hosts.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CJudgeTmpRouteEntry & tmpRouteEntry = m_ArrItems[nIndex].m_Hosts.GetNext(pos);
		if (tmpRouteEntry.m_HopFrom.m_pHost == m_ArrItems[nIndex].m_pCenterHost)
		{
			continue;
		}
		tmpPair.m_pItem = &m_ArrItems[tmpRouteEntry.m_HopFrom.m_pHost->m_nId];
		tmpPair.m_Group = posLast;
		detectedHosts[tmpPair.m_pItem] = tmpPair;
		searchingHosts.AddTail(tmpPair);
		++nDirectNeighbourCount;
		++nAllNeighbourCount;
	}

	while (searchingHosts.GetSize() > 0)
	{
		CJudgeDijPair searchingPair = searchingHosts.RemoveHead();
		m_ArrItems[nIndex].m_HopDestinations[searchingPair.m_pItem->m_pCenterHost->m_nId] = searchingPair.m_Group;
		CList<CJudgeTmpRouteEntry> & currentNeighbours = searchingPair.m_pItem->m_Hosts;
		pos = currentNeighbours.GetHeadPosition();
		while (pos)
		{
			CReceiverReportItem * pNextNeighbour = &m_ArrItems[currentNeighbours.GetNext(pos).GetHopFromId()];
			CJudgeDijPair rValue;
			if (detectedHosts.Lookup(pNextNeighbour, rValue))
			{
				continue;
			}
			tmpPair.m_pItem = pNextNeighbour;
			tmpPair.m_Group = searchingPair.m_Group;
			detectedHosts[tmpPair.m_pItem] = tmpPair;
			searchingHosts.AddTail(tmpPair);
			++nAllNeighbourCount;
		}
	}
	if (nAllNeighbourCount > nDirectNeighbourCount)
	{
		m_bHasMultiNeighbours = TRUE;
	}
	m_ArrItems[nIndex].m_nNeighbourCount = nAllNeighbourCount;
	m_ArrItems[nIndex].GenerateAllNeighbourArr();
}

void CMsgCntJudgeReceiverReport::ShowAllDijFor(int nIndex)
{
	CReceiverReportItem & recentItem = m_ArrItems[nIndex];
	if (recentItem.m_Hosts.GetSize() <= 1)
	{
		return;
	}
	CString strOut;
	strOut.Format(_T("\n--%d"), nIndex);
	OutputDebugString(strOut);

	int rKey;
	POSITION rValue;

	POSITION pos = recentItem.m_HopDestinations.GetStartPosition();
	while (pos)
	{
		recentItem.m_HopDestinations.GetNextAssoc(pos, rKey, rValue);
		strOut.Format(_T("\n\t%d -> %d"), rKey, recentItem.m_Hosts.GetAt(rValue).m_HopFrom.m_pHost->m_nId);
		OutputDebugString(strOut);
	}
}

CJudgeTmpRouteEntry::CJudgeTmpRouteEntry()
{

}

CJudgeTmpRouteEntry::CJudgeTmpRouteEntry(const CJudgeTmpRouteEntry & src)
{

}

int CJudgeTmpRouteEntry::GetHopFromId() const
{
	int nId = m_HopFrom.m_pHost->m_nId;
	return nId;
}

CJudgeTmpRouteEntry & CJudgeTmpRouteEntry::operator=(const CJudgeTmpRouteEntry & src)
{
	m_HopFrom = src.m_HopFrom;
	return *this;
}
