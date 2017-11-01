// ConnectionJudge.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "ConnectionJudge.h"
#include "PositionForecast.h"
#include "RoadNet.h"
#include "MsgCntJudgeReceiverReport.h"
#include "HostEngine.h"



// CConnectionJudge

IMPLEMENT_DYNCREATE(CConnectionJudge, CWinThread)

CConnectionJudge::CConnectionJudge()
	: m_pData(NULL)
	, m_pForecast(NULL)
	, m_pEngine(NULL)
	, OutRangeCount(0)
	, m_timeCost(0)
	, m_timeCost1(0)
	, nTimes(0)
{
}

CConnectionJudge::~CConnectionJudge()
{
}

BOOL CConnectionJudge::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CConnectionJudge::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

void CConnectionJudge::SetData(CRoadNet * pData)
{
	m_pData = pData;
}

void CConnectionJudge::SetForecast(CPositionForecast * pForecast)
{
	m_pForecast = pForecast;
}

void CConnectionJudge::SetEngine(CHostEngine * pEngine)
{
	m_pEngine = pEngine;
}

void CConnectionJudge::OnFinishedForecastOnce(SIM_TIME lnSimTime)
{
// 	SIM_TIME * pMsg = new SIM_TIME(lnSimTime);
// 	PostThreadMessage(MSG_ID_JUDGE_FORECAST_OK, (WPARAM)pMsg, 0);
}

void CConnectionJudge::OnFinishedOneForecast(WPARAM wParam, LPARAM lParam)
{
	SIM_TIME * pMsg = (SIM_TIME *)wParam;
	SIM_TIME lnSimTime = *pMsg;
	delete pMsg;
	pMsg = NULL;
}

void CConnectionJudge::OnNewSend(WPARAM wParam, LPARAM lParam)
{
	ULONGLONG a = GetTickCount64();
	CMsgNewSendJudge * pMsg = (CMsgNewSendJudge *)wParam;
	if (!pMsg->m_bFullJudge)
	{
		ASSERT(0);
		return;
	}
	if (pMsg->m_Items.GetSize() == 0 && pMsg->m_bFullJudge == FALSE)
	{
		return;
	}
	double fSecondId = pMsg->m_fSecondId;
	POSITION posOccupyReport = NULL;
	CMsgPosFrcstReport * pReport = m_pForecast->GetReport(fSecondId, LONG_MAX, posOccupyReport);
	if (!pReport)
	{
		ASSERT(NULL);
		return;
	}
	SIM_TIME rpttime = pReport->m_lnSimTime;
	BOOL bO = m_pForecast->IsOccupied(posOccupyReport);

	ULONGLONG b = GetTickCount64();
	CMsgCntJudgeReceiverReport * pJRR = new CMsgCntJudgeReceiverReport();
	pJRR->m_lnTime = fSecondId;
	pJRR->m_bFullReport = pMsg->m_bFullJudge;
	int nReportSize = m_pData->m_allHosts.GetSize();
	pJRR->m_ArrItems.SetSize(nReportSize);

	CMsgNewJudgeItem judgingItem;
	judgingItem.m_fRadius = pMsg->m_fRadius;
	judgingItem.m_fSecondId = fSecondId;

	for (int i = 0; i < nReportSize; ++i)
	{
		judgingItem.m_pHost = m_pData->m_allHosts[i];
		CReceiverReportItem reportItem;
		JudgeItem(judgingItem, pReport, reportItem);
		pJRR->m_ArrItems[i] = reportItem;
	}

	m_pForecast->GiveBackReport(posOccupyReport);

	if (m_pEngine)
	{
		m_pEngine->PostThreadMessage(MSG_ID_ENGINE_JUDGE_OK, (WPARAM)pJRR, 0);
	}

	delete pMsg;
	ULONGLONG c = GetTickCount64();
	nTimes++;
	m_timeCost += (b - a);
	m_timeCost1 += (c - b);
	ULONGLONG AVE1 = m_timeCost / nTimes;
	ULONGLONG AVE2 = m_timeCost1 / nTimes;
	int k = c - b;
	if (nTimes > 400)
	{
		int lll = 3;
	}
}

void CConnectionJudge::JudgeItem(const CMsgNewJudgeItem & item, CMsgPosFrcstReport * pReport, CReceiverReportItem & ret)
{
	ret.m_pCenterHost = item.m_pHost;
	ret.m_Hosts.RemoveAll();

	int nBlockCount;
	SIM_TIME lnPredictTime;
	SIM_TIME lnHalfBlockTime;
	double fCommunicationRadius;
	m_pData->CalculateBlockAndPredictTime(nBlockCount, lnPredictTime, lnHalfBlockTime, item.m_fRadius);

	if (!pReport)
	{
		ASSERT(NULL);
	}
	SIM_TIME lnDiffer = item.m_fSecondId - pReport->m_lnSimTime;
	int nLimit = nBlockCount;
	ASSERT(nLimit >= 0);
	if (lnDiffer > lnPredictTime)
	{
		nLimit += (lnDiffer - lnPredictTime) / lnHalfBlockTime + 1;
	}
	//ASSERT(nLimit <= 1);
	if (nLimit > 1)
	{
		++OutRangeCount;
	}
	//ASSERT(OutRangeCount < 1000);
	if (nTimes > 1100)
	{
		//ASSERT(nLimit <= 2);
	}
	//nLimit = 0;


	CDoublePoint centerPosition = item.m_pHost->m_schedule.GetPosition(item.m_fSecondId);
	DWORD OriHashValue = m_pData->GetHashValue(centerPosition.m_X, centerPosition.m_Y);
	if (!m_pData->HasHash(centerPosition))
	{
		item.m_pHost->m_schedule.GetPosition(item.m_fSecondId);
	}
	BOOL bFindSelf = FALSE;
	for (int i = -nLimit; i <= nLimit; ++i)
	{
		for (int j = -nLimit; j <= nLimit; ++j)
		{
			CDoublePoint testPosition;
			testPosition.m_X = centerPosition.m_X + i * m_pData->GetHashInterval();
			testPosition.m_Y = centerPosition.m_Y + j * m_pData->GetHashInterval();
			if (!m_pData->HasHash(testPosition))
			{
				continue;
			}
			DWORD HashValue = m_pData->GetHashValue(testPosition.m_X, testPosition.m_Y);
			CHostReference tmpReference;
			if (pReport->m_Reference.Lookup(HashValue, tmpReference))
			{
				POSITION pos = tmpReference.m_Hosts.GetHeadPosition();
				while (pos)
				{
					CHostGui tmpTarget = tmpReference.m_Hosts.GetNext(pos);
					CDoublePoint targetPosition = tmpTarget.m_pHost->GetPosition(item.m_fSecondId);
					double fDistance = CDoublePoint::GetDistance(targetPosition, centerPosition);
					if (fDistance <= item.m_fRadius)
					{
						if (tmpTarget.m_pHost == item.m_pHost)
						{
							bFindSelf = TRUE;
						}
						tmpTarget.m_Position = targetPosition;
						ret.m_Hosts.AddTail(tmpTarget);
					}
				}
			}

		}
	}
// 	if (bFindSelf == FALSE)
// 	{
// 		CHost * pLookingFor = item.m_pHost;
// 		CHostGui findGui;
// 		int nKey = pReport->FindHost(pLookingFor, findGui);
// 		int k = 3;
// 	}
}

BEGIN_MESSAGE_MAP(CConnectionJudge, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_JUDGE_FORECAST_OK, OnFinishedOneForecast)
	ON_THREAD_MESSAGE(MSG_ID_JUDGE_NEW_SEND, OnNewSend)
END_MESSAGE_MAP()


// CConnectionJudge 消息处理程序
