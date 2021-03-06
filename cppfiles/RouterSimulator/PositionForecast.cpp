// PositionForecast.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "PositionForecast.h"
#include "RoadNet.h"
#include "PositionForecastUser.h"


CMsgPosFrcstNewTime::CMsgPosFrcstNewTime()
	: m_lnSimTime(0)
{

}

CMsgPosFrcstNewTime::CMsgPosFrcstNewTime(const CMsgPosFrcstNewTime & src)
{
	*this = src;
}

CMsgPosFrcstNewTime & CMsgPosFrcstNewTime::operator=(const CMsgPosFrcstNewTime & src)
{
	m_lnSimTime = src.m_lnSimTime;
	return *this;
}


CMsgPosFrcstComplete::CMsgPosFrcstComplete()
	: m_lnSimTime(0)
{

}

CMsgPosFrcstComplete::CMsgPosFrcstComplete(const CMsgPosFrcstComplete & src)
{
	*this = src;
}

CMsgPosFrcstComplete & CMsgPosFrcstComplete::operator=(const CMsgPosFrcstComplete & src)
{
	m_lnSimTime = src.m_lnSimTime;
	return *this;
}

CMsgPosFrcstReport::CMsgPosFrcstReport()
	: m_lnSimTime(0.0)
{

}

CMsgPosFrcstReport::CMsgPosFrcstReport(const CMsgPosFrcstReport & src)
{
	*this = src;
}


int CMsgPosFrcstReport::FindHost(CHost * pHost, CHostGui & hostGui)
{
	POSITION pos = m_Reference.GetStartPosition();
	while (pos)
	{
		int nKey;
		CHostReference value;
		m_Reference.GetNextAssoc(pos, nKey, value);
		POSITION posList = value.m_Hosts.GetHeadPosition();
		while (posList)
		{
			if (value.m_Hosts.GetAt(posList).m_pHost == pHost)
			{
				hostGui = value.m_Hosts.GetAt(posList);
				return nKey;
			}
			value.m_Hosts.GetNext(posList);
		}
	}
	return -1;
}

CMsgPosFrcstReport & CMsgPosFrcstReport::operator=(const CMsgPosFrcstReport & src)
{
	m_lnSimTime = src.m_lnSimTime;
	m_Reference.RemoveAll();
	POSITION pos = src.m_Reference.GetStartPosition();
	while (pos)
	{
		int nKey;
		CHostReference value;
		src.m_Reference.GetNextAssoc(pos, nKey, value);
		m_Reference.SetAt(nKey, value);
	}
	return *this;
}


// CPositionForecast

IMPLEMENT_DYNCREATE(CPositionForecast, CWinThread)

CPositionForecast::CPositionForecast()
	: nTestMutex(0)
{
}

CPositionForecast::~CPositionForecast()
{
}

BOOL CPositionForecast::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CPositionForecast::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

void CPositionForecast::SetData(CRoadNet * pNet)
{
	m_pData = pNet;
}

void CPositionForecast::LockReports()
{
	m_MutexReport.Lock();
	ASSERT(nTestMutex++ == 0);
}

void CPositionForecast::UnlockReports()
{
	--nTestMutex;
	m_MutexReport.Unlock();
}

void CPositionForecast::AddUser(CPositionForecastUser * pUser)
{
	m_MutexReport.Lock();
	bool bFound = false;
	POSITION pos = m_Users.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CPositionForecastUser * pPin = m_Users.GetNext(pos);
		if (pPin == pUser)
		{
			bFound = true;
			m_MutexReport.Unlock();
			return;
		}
	}
	m_Users.AddTail(pUser);
	m_MutexReport.Unlock();
}

void CPositionForecast::DelUser(CPositionForecastUser * pUser)
{
	m_MutexReport.Lock();
	POSITION pos = m_Users.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CPositionForecastUser * pPin = m_Users.GetNext(pos);
		if (pPin == pUser)
		{
			m_Users.RemoveAt(posLast);
		}
	}
	m_MutexReport.Unlock();
}

CMsgPosFrcstReport * CPositionForecast::GetReport(SIM_TIME lnSimTime, SIM_TIME lnDiffer, POSITION & posRet)
{
	LockReports();
// 	int nSize = m_Reports.GetSize();
// 	ASSERT(nSize < 60);
	POSITION pos = m_Reports.GetHeadPosition(), posLast;
	CMsgPosFrcstReport * pRet = NULL;
	SIM_TIME lnMinDiffer = INT_MAX;
	while (pos)
	{
		CMsgPosFrcstReport * pCurrent = NULL;
		posLast = pos;
		pCurrent = m_Reports.GetNext(pos);
		if (lnSimTime >= pCurrent->m_lnSimTime)
		{
			if (lnSimTime - pCurrent->m_lnSimTime < lnMinDiffer)
			{
				lnMinDiffer = lnSimTime - pCurrent->m_lnSimTime;
				pRet = pCurrent;
				posRet = posLast;
			}
		}
		else
		{
			if (pCurrent->m_lnSimTime - lnSimTime < lnMinDiffer)
			{
				lnMinDiffer = lnSimTime - pCurrent->m_lnSimTime;
				pRet = pCurrent;
				posRet = posLast;
			}
		}
	}
	if (lnMinDiffer > lnDiffer)
	{
		UnlockReports();
		return NULL;
	}
	else
	{
		int mapValue;
		if (m_OccupyRecord.Lookup(posRet, mapValue))
		{
			m_OccupyRecord[posRet] = ++mapValue;
		}
		else
		{
			m_OccupyRecord[posRet] = 1;
		}
		UnlockReports();
		return pRet;
	}
}

void CPositionForecast::GiveBackReport(POSITION pos)
{
	LockReports();
	int mapValue;
	if (m_OccupyRecord.Lookup(pos, mapValue))
	{
		m_OccupyRecord[pos] = mapValue - 1;
		if (mapValue == 1)
		{
			m_OccupyRecord.RemoveKey(pos);
		}
	}
	else
	{
		ASSERT(0);
	}
	UnlockReports();
}

void CPositionForecast::OnForecastNewTime(WPARAM wParam, LPARAM lParam)
{
	CMsgPosFrcstNewTime * pMsg = (CMsgPosFrcstNewTime *)wParam;
	if (!pMsg)
	{
		ASSERT(0);
		return;
	}

	POSITION pos = NULL;
	CMsgPosFrcstReport * pReport = GetReport(pMsg->m_lnSimTime, 0, pos);
	if (!pReport)
	{
		DoNewForecast(pMsg->m_lnSimTime);
	}
	else
	{
		GiveBackReport(pos);
	}
	delete pMsg;
}

void CPositionForecast::OnForecastComplete(WPARAM wParam, LPARAM lParam)
{
	int nBeforeSecond = (int)wParam;
	DeleteRecords(nBeforeSecond);
}

void CPositionForecast::OnForecastRemoveAll(WPARAM wParam, LPARAM lParam)
{
	LockReports();
	POSITION pos = m_Reports.GetHeadPosition();
	CMsgPosFrcstReport * pToBeDelete = NULL;
	while (pos)
	{
		pToBeDelete = m_Reports.GetNext(pos);
		delete pToBeDelete;
	}
	m_Reports.RemoveAll();
	UnlockReports();
}

void CPositionForecast::DeleteRecords(SIM_TIME lnSimTimeBefore)
{
	LockReports();
	int nsize = m_Reports.GetSize();
	//ASSERT(nsize < 30);
	POSITION pos = m_Reports.GetHeadPosition(), posLast;
	CMsgPosFrcstReport * pNext = NULL;
	CMsgPosFrcstReport * pToBeDelete = NULL;
	while (pos)
	{
		posLast = pos;
		pToBeDelete = m_Reports.GetNext(pos);
		if (!pos)
		{
			break;
		}
		pNext = m_Reports.GetAt(pos);
		if (lnSimTimeBefore - pNext->m_lnSimTime > 0)
		{
			if (IsOccupied(posLast))
			{
				break;
			}
			delete pToBeDelete;
			m_Reports.RemoveAt(posLast);
		}
		else
		{
			break;
		}
	}
	UnlockReports();
}

void CPositionForecast::DoNewForecast(SIM_TIME lnSimTime)
{
	CMsgPosFrcstReport * pNewReport = new CMsgPosFrcstReport();
	pNewReport->m_lnSimTime = lnSimTime;
	pNewReport->m_Reference.InitHashTable(157);
	CHostGui newHostPosition;

	for (int i = 0; i < m_pData->m_allHosts.GetSize(); ++i)
	{
		if (!m_pData->m_allHosts[i]->m_schedule.IsScheduleAvailable())
		{
			ASSERT(0);
			continue;
		}
		newHostPosition.m_Position = m_pData->m_allHosts[i]->GetPosition(lnSimTime);
		newHostPosition.m_pHost = m_pData->m_allHosts[i];
		DWORD nHash = m_pData->GetHashValue(&newHostPosition.m_Position);
		pNewReport->m_Reference[nHash].AddNewHost(newHostPosition);
	}

	LockReports();
	m_Reports.AddTail(pNewReport);
	UnlockReports();
	POSITION pos = m_Users.GetHeadPosition();
	while (pos)
	{
		CPositionForecastUser * pUser = m_Users.GetNext(pos);
		pUser->OnFinishedForecastOnce(pNewReport->m_lnSimTime);
	}
}

BOOL CPositionForecast::IsOccupied(POSITION pos)
{
	int mapValue;
	if (m_OccupyRecord.Lookup(pos, mapValue))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BEGIN_MESSAGE_MAP(CPositionForecast, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_POS_FRCST_NEW_TIME, OnForecastNewTime)
	ON_THREAD_MESSAGE(MSG_ID_POS_FRCST_COMPLETE, OnForecastComplete)
	ON_THREAD_MESSAGE(MSG_ID_POS_FRCST_REMOVEALL, OnForecastRemoveAll)
END_MESSAGE_MAP()


// CPositionForecast 消息处理程序
