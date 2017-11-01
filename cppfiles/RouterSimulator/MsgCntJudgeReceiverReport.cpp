#include "stdafx.h"
#include "MsgCntJudgeReceiverReport.h"
#include "Host.h"


CReceiverReportItem::CReceiverReportItem()
	: m_bUnicast(FALSE)
{

}


CReceiverReportItem::CReceiverReportItem(const CReceiverReportItem & src)
{
	*this = src;
}

CReceiverReportItem::~CReceiverReportItem()
{

}

CReceiverReportItem & CReceiverReportItem::operator=(const CReceiverReportItem & src)
{
	m_bUnicast = src.m_bUnicast;
	m_pCenterHost = src.m_pCenterHost;
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
	if (m_bHasMultiNeighbours)
	{
		ShowAllDij();
	}
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
		m_ArrItems[nIndex].m_Hosts.GetAt(searchingPair.m_Group).m_HopDestinations.AddTail(searchingPair.m_pItem);
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
	POSITION posDirNeighbor = recentItem.m_Hosts.GetHeadPosition();
	while (posDirNeighbor)
	{
		CList<CReceiverReportItem *> & multiNeighbors = recentItem.m_Hosts.GetNext(posDirNeighbor).m_HopDestinations;
		if (multiNeighbors.GetSize() == 0)
		{
			continue;
		}
		POSITION posMulti = multiNeighbors.GetHeadPosition();
		int nDirId = multiNeighbors.GetNext(posMulti)->m_pCenterHost->m_nId;
		if (nDirId == m_ArrItems[nIndex].m_pCenterHost->m_nId)
		{
			continue;
		}
		strOut.Format(_T("\n\t%d : "), nDirId);
		OutputDebugString(strOut);
		while (posMulti)
		{
			strOut.Format(_T("%d,"), multiNeighbors.GetNext(posMulti)->m_pCenterHost->m_nId);
			OutputDebugString(strOut);
		}
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
	m_HopDestinations.RemoveAll();
	POSITION pos = src.m_HopDestinations.GetHeadPosition();
	while (pos)
	{
		m_HopDestinations.AddTail(src.m_HopDestinations.GetNext(pos));
	}
	return *this;
}
