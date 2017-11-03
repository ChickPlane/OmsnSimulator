#include "stdafx.h"
#include "Host.h"


CHost::CHost()
	: m_pProtocol(NULL)
	, m_nId(0)
	, m_pDirNeighbours(NULL)
	, m_nDirNeighbourCount(0)
	, m_pAllNeighbours(NULL)
	, m_nAllNeighbourCount(0)
	, m_pCurrentReport(NULL)
	, m_bAnyOneNearby(FALSE)
	, m_bDifferentFromPrev(TRUE)
{
}


CHost::CHost(const CHost & src)
{
	*this = src;
}

CHost & CHost::operator=(const CHost & src)
{
	m_schedule = src.m_schedule;
	m_pProtocol = NULL;
	m_nId = src.m_nId;
	return *this;
}

CHost::~CHost()
{
	Reset();
}

double CHost::GetDefaultSpeed() const
{
	return m_fSpeed;
}

void CHost::SetSpeed(double fSpeed)
{
	m_fSpeed = fSpeed;
}

void CHost::OnHearMsg(const CYell * pYell)
{
	if (m_pProtocol)
	{
		m_pProtocol->OnReceivedMsg(pYell);
	}
}

void CHost::UpdateNetworkLocations(const CMsgCntJudgeReceiverReport* pWholeReport)
{
	const CReceiverReportItem & reportItem = pWholeReport->m_ArrItems[m_pProtocol->GetHostId()];
	m_bDifferentFromPrev = IsDifferentList(reportItem);
	m_pCurrentReport = pWholeReport;
	if (!reportItem.IsAnyOneNearby())
	{
		// No host nearby
		m_bAnyOneNearby = FALSE;
		return;
	}
	m_bAnyOneNearby = TRUE;
	m_nDirNeighbourCount = reportItem.m_nDirNeighbourCount;
	m_nAllNeighbourCount = reportItem.m_nNeighbourCount;
	if (m_pDirNeighbours)
	{
		delete[]m_pDirNeighbours;
		m_pDirNeighbours = NULL;
	}
	if (m_pAllNeighbours)
	{
		delete[]m_pAllNeighbours;
		m_pAllNeighbours = NULL;
	}

	if (m_nDirNeighbourCount > 0)
	{
		m_pDirNeighbours = new int[m_nDirNeighbourCount];
		memcpy(m_pDirNeighbours, reportItem.m_pDirNeighbours, sizeof(int) * m_nDirNeighbourCount);
	}
	if (m_nAllNeighbourCount > 0)
	{
		m_pAllNeighbours = new int[m_nAllNeighbourCount];
		memcpy(m_pAllNeighbours, reportItem.m_pAllNeighbours, sizeof(int) * m_nAllNeighbourCount);
	}
}

void CHost::OnEnterNewTimePeriod()
{
	if (m_pProtocol)
	{
		m_pProtocol->OnEngineConnection(m_bAnyOneNearby, m_bDifferentFromPrev);
	}
}

void CHost::OnPackageArrived(const CQueryMission * pMission)
{
	m_nReceivedMsgs[pMission->m_nMissionId] = 1;
	ASSERT(m_nId == pMission->m_nMissionId);

	CString strOut;
	int nToId = m_nId;
	strOut.Format(_T("Receive [%7d] DESTINATION (%d) !!!"), pMission->m_nMissionId, nToId);
	m_pProtocol->WriteLog(strOut);
}

bool CHost::IsReceivedPackage(const CQueryMission * pMission)
{
	int nValue;
	return true == m_nReceivedMsgs.Lookup(pMission->m_nMissionId, nValue);
}

void CHost::GetAllCarryingMessages(CMsgShowInfo & allMessages)
{
	m_pProtocol->GetAllCarryingMessages(allMessages);
}

const CMsgCntJudgeReceiverReport* CHost::GetRecentReport() const
{
	return m_pCurrentReport;
}

CDoublePoint CHost::GetPosition(SIM_TIME lnSimTime) const
{
	return m_schedule.GetPosition(lnSimTime);
}

void CHost::Reset()
{
	m_schedule.Reset();
	if (m_pProtocol)
	{
		delete m_pProtocol;
		m_pProtocol = NULL;
	}
	m_nReceivedMsgs.RemoveAll();
}

void CHost::GetInfo(CHostInfo & ret)
{
	ret.Reset();
	ret.m_nHostId = m_nId;
}

BOOL CHost::IsDifferentList(const CReceiverReportItem & reportItem)
{
	if (m_bAnyOneNearby == FALSE && reportItem.IsAnyOneNearby())
	{
		return reportItem.IsAnyOneNearby();
	}
	BOOL bRet = FALSE;
	if (m_nDirNeighbourCount != reportItem.m_nDirNeighbourCount)
	{
		bRet = TRUE;
	}
	else if (memcmp(m_pDirNeighbours, reportItem.m_pDirNeighbours, sizeof(int) * m_nDirNeighbourCount) != 0)
	{
		bRet = TRUE;
	}

	if (m_nAllNeighbourCount != reportItem.m_nNeighbourCount)
	{
		bRet = TRUE;
	}
	else if (memcmp(m_pAllNeighbours, reportItem.m_pAllNeighbours, sizeof(int) * m_nAllNeighbourCount) != 0)
	{
		bRet = TRUE;
	}
	return bRet;
}
