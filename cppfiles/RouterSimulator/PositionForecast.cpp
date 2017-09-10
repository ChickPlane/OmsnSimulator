// PositionForecast.cpp : ʵ���ļ�
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
{
}

CPositionForecast::~CPositionForecast()
{
}

BOOL CPositionForecast::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CPositionForecast::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

void CPositionForecast::SetData(CRoadNet * pNet)
{
	m_pData = pNet;
}

void CPositionForecast::LockReports()
{
	m_MutexReport.Lock();
}

void CPositionForecast::UnlockReports()
{
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

CMsgPosFrcstReport * CPositionForecast::GetReport(SIM_TIME lnSimTime, SIM_TIME lnDiffer)
{
	LockReports();
	POSITION pos = m_Reports.GetHeadPosition();
	CMsgPosFrcstReport * pRet = NULL;
	while (pos)
	{
		pRet = m_Reports.GetNext(pos);
		if (lnSimTime - pRet->m_lnSimTime >= 0 && lnSimTime - pRet->m_lnSimTime <= lnDiffer)
		{
			UnlockReports();
			return pRet;
		}
	}
	UnlockReports();
	return NULL;
}

void CPositionForecast::OnForecastNewTime(WPARAM wParam, LPARAM lParam)
{
	CMsgPosFrcstNewTime * pMsg = (CMsgPosFrcstNewTime *)wParam;
	if (!pMsg)
	{
		return;
	}

	CMsgPosFrcstReport * pReport = GetReport(pMsg->m_lnSimTime, 0);
	if (!pReport)
	{
		DoNewForecast(pMsg->m_lnSimTime);
	}
	delete pMsg;
}

void CPositionForecast::OnForecastComplete(WPARAM wParam, LPARAM lParam)
{
	CMsgPosFrcstComplete * pMsg = (CMsgPosFrcstComplete*)wParam;
	if (!pMsg)
	{
		return;
	}
	DeleteRecords(pMsg->m_lnSimTime);
	delete pMsg;
}

void CPositionForecast::DeleteRecords(SIM_TIME lnSimTimeBefore)
{
	LockReports();
	POSITION pos = m_Reports.GetHeadPosition(), posLast;
	CMsgPosFrcstReport * pRet = NULL;
	while (pos)
	{
		posLast = pos;
		pRet = m_Reports.GetNext(pos);
		if (lnSimTimeBefore - pRet->m_lnSimTime > 0)
		{
			delete m_Reports.GetAt(posLast);
			m_Reports.RemoveAt(posLast);
		}
	}
	UnlockReports();
}

void CPositionForecast::DoNewForecast(SIM_TIME lnSimTime)
{
	CMsgPosFrcstReport * pNewReport = new CMsgPosFrcstReport();
	pNewReport->m_lnSimTime = lnSimTime;
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

BEGIN_MESSAGE_MAP(CPositionForecast, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_POS_FRCST_NEW_TIME, OnForecastNewTime)
	ON_THREAD_MESSAGE(MSG_ID_POS_FRCST_COMPLETE, OnForecastComplete)
END_MESSAGE_MAP()


// CPositionForecast ��Ϣ�������
