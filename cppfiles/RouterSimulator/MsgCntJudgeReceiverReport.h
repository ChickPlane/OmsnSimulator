#pragma once
#include "HostGui.h"
#include "SimulatorCommon.h"

class CReceiverReportItem;

class CJudgeDijPair
{
public:
	CReceiverReportItem * m_pItem;
	POSITION m_Group;
};

class CJudgeTmpRouteEntry
{
public:
	CJudgeTmpRouteEntry();
	CJudgeTmpRouteEntry(const CJudgeTmpRouteEntry & src);
	CJudgeTmpRouteEntry & operator = (const CJudgeTmpRouteEntry & src);

	int GetHopFromId() const;

	CHostGui m_HopFrom;
};

class CReceiverReportItem
{
public:
	CReceiverReportItem();
	CReceiverReportItem(const CReceiverReportItem & src);
	CReceiverReportItem & operator = (const CReceiverReportItem & src);
	~CReceiverReportItem();

	void GenerateDirNeighbourArr();
	void GenerateAllNeighbourArr();
	CHost * GetNextHop(USERID nDestinationId) const;
	BOOL IsAnyOneNearby() const;

	BOOL m_bUnicast;
	CList<CJudgeTmpRouteEntry> m_Hosts;
	CHost * m_pCenterHost;
	CDoublePoint m_CenterLocation;
	int * m_pDirNeighbours;
	int m_nDirNeighbourCount;
	int * m_pAllNeighbours;
	int m_nNeighbourCount;
	CMap<int, int, POSITION, POSITION> m_HopDestinations;
};

class CMsgCntJudgeReceiverReport
{
public:
	CMsgCntJudgeReceiverReport();
	CMsgCntJudgeReceiverReport(const CMsgCntJudgeReceiverReport & src);
	CMsgCntJudgeReceiverReport & operator = (const CMsgCntJudgeReceiverReport & src);
	~CMsgCntJudgeReceiverReport();
	
	void RunDij();
	void ShowAllDij();

	BOOL m_bFullReport;
	BOOL m_bDijEnable;
	BOOL m_bHasMultiNeighbours;
	SIM_TIME m_lnTime;
	CArray<CReceiverReportItem> m_ArrItems;

protected:
	void RunDijFor(int nIndex);
	void ShowAllDijFor(int nIndex);
};

