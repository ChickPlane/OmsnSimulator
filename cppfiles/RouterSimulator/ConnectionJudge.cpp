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
	CMsgNewSendJudge * pMsg = (CMsgNewSendJudge *)wParam;
	if (pMsg->m_Items.GetSize() == 0)
	{
		return;
	}
	double fSecondId = pMsg->GetSecondId();
	POSITION posOccupyReport = NULL;
	CMsgPosFrcstReport * pReport = m_pForecast->GetReport(fSecondId, LONG_MAX, posOccupyReport);
	if (!pReport)
	{
		ASSERT(NULL);
		return;
	}
	SIM_TIME rpttime = pReport->m_lnSimTime;
	BOOL bO = m_pForecast->IsOccupied(posOccupyReport);

	CMsgCntJudgeReceiverReport * pJRR = new CMsgCntJudgeReceiverReport();
	pJRR->m_lnTime = fSecondId;
	pJRR->m_bFullReport = pMsg->m_bFullJudge;

	POSITION pos = pMsg->m_Items.GetHeadPosition();
	while (pos)
	{
		CMsgNewJudgeItem & item = pMsg->m_Items.GetNext(pos);
		CReceiverReportItem reportItem;
		if (!pJRR->m_Items.Lookup(item.m_pHost, reportItem))
		{
			JudgeItem(item, pReport, reportItem);
			pJRR->m_Items.SetAt(item.m_pHost, reportItem);
		}
	}

	m_pForecast->GiveBackReport(posOccupyReport);

	if (m_pEngine)
	{
		m_pEngine->PostThreadMessage(MSG_ID_ENGINE_JUDGE_OK, (WPARAM)pJRR, 0);
	}

	delete pMsg;
}

void CConnectionJudge::JudgeItem(const CMsgNewJudgeItem & item, CMsgPosFrcstReport * pReport, CReceiverReportItem & ret)
{
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
	int nLimit = nBlockCount - 1;
	ASSERT(nLimit >= 0);
	if (lnDiffer > lnPredictTime)
	{
		nLimit += (lnDiffer - lnPredictTime) / lnHalfBlockTime;
	}
	ASSERT(nLimit <= 3);


	CDoublePoint centerPosition = item.m_pHost->m_schedule.GetPosition(item.m_fSecondId);
	for (int i = -nLimit; i <= nLimit; ++i)
	{
		for (int j = -nLimit; j <= nLimit; ++j)
		{
			double fX = centerPosition.m_X + i * m_pData->GetHashInterval();
			double fY = centerPosition.m_Y + j * m_pData->GetHashInterval();
			DWORD HashValue = m_pData->GetHashValue(fX, fY);
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
						tmpTarget.m_Position = targetPosition;
						ret.m_Hosts.AddTail(tmpTarget);
					}
				}
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CConnectionJudge, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_JUDGE_FORECAST_OK, OnFinishedOneForecast)
	ON_THREAD_MESSAGE(MSG_ID_JUDGE_NEW_SEND, OnNewSend)
END_MESSAGE_MAP()


// CConnectionJudge 消息处理程序

double CMsgNewSendJudge::GetSecondId()
{
	if (m_Items.GetSize() == 0)
	{
		ASSERT(0);
		return 0;
	}

	double fSecondId = m_Items.GetHead().m_fSecondId;
	POSITION pos = m_Items.GetHeadPosition();
	while (pos)
	{
		if (fSecondId != m_Items.GetNext(pos).m_fSecondId)
		{
			ASSERT(0);
			return 0;
		}
	}
	return fSecondId;
}
