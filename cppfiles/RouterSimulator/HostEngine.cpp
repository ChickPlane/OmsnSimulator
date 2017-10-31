// HostEngine.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "HostEngine.h"
#include "RoadNet.h"
#include "HostGui.h"
#include "MapGui.h"
#include "PositionForecast.h"
#include "TransmitionRecord.h"
#include "ConnectionJudge.h"
#include "MsgCntJudgeReceiverReport.h"
#include "Yell.h"
#include "Host.h"
#include "RoutingProtocol.h"



// CHostEngine

IMPLEMENT_DYNCREATE(CHostEngine, CWinThread)

CHostEngine::CHostEngine()
	: m_lnSimTimeMillisecond(0)
	, m_lnExpectSimMillisecPerActSec(15000)
	, m_pRoadNet(NULL)
	, m_pMapGui(NULL)
	, m_bPaused(true)
	, m_lnLastForecastSimTime(INT_MIN)
	, m_lnLastForecastedSimTime(INT_MIN)
	, m_nMsgId(0)
	, m_nJudgeMax(7)
#ifdef DEBUG
	, m_bEnableMonitor(FALSE)
	, m_blimitedSpeed(FALSE)
#else
	, m_bEnableMonitor(FALSE)
	, m_blimitedSpeed(FALSE)
#endif
	, m_bWaitingActualTime(TRUE)
	, m_bCheckingEvents(FALSE)
	, m_pJudgeMsg(NULL)
	, m_nJudgeRunTime(0)
	, m_bJudgingUnicast(FALSE)
	, m_nMsgCount(0)
	, m_nJudgeOkMsgCount(0)
	, m_nBusyJudgeThreadCount(0)
{
	m_lnEventCheckBoundary = GetPeriodDefaultInterval();
}

CHostEngine::~CHostEngine()
{
}

BOOL CHostEngine::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	m_pForecastThread = (CPositionForecast*)AfxBeginThread(RUNTIME_CLASS(CPositionForecast));
	m_pForecastThread->SetData(m_pRoadNet);
	m_pForecastThread->AddUser(this);

	for (int i = 0; i < m_nJudgeMax; ++i)
	{
		CConnectionJudge * pJudge = (CConnectionJudge*)AfxBeginThread(RUNTIME_CLASS(CConnectionJudge));

		m_pForecastThread->AddUser(pJudge);
		pJudge->SetData(m_pRoadNet);
		pJudge->SetEngine(this);
		pJudge->SetForecast(m_pForecastThread);

		m_JudgeList.Add(pJudge);
	}
	//m_pJudge = (CConnectionJudge*)AfxBeginThread(RUNTIME_CLASS(CConnectionJudge));

	ASSERT(m_pRoadNet);

	//m_tt.StartTest();

	return TRUE;
}

int CHostEngine::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	CleanSelf();
	DWORD ExitCode = 0;
	if (m_pForecastThread)
	{
		GetExitCodeThread(m_pForecastThread->m_hThread, &ExitCode);
		TerminateThread(m_pForecastThread->m_hThread, ExitCode);
	}
	for (int i = 0; i < m_JudgeList.GetSize(); ++i)
	{
		GetExitCodeThread(m_JudgeList[i]->m_hThread, &ExitCode);
		TerminateThread(m_JudgeList[i]->m_hThread, ExitCode);
	}
	return CWinThread::ExitInstance();
}

void CHostEngine::SetValue(CRoadNet * pRoadNet, CMapGui * pMapGui)
{
	m_pRoadNet = pRoadNet;
	m_pMapGui = pMapGui;
	m_Connections.Reset(m_pRoadNet->m_allHosts.GetSize());
}

void CHostEngine::BreakMapGui()
{
	m_pMapGui = NULL;
}

void CHostEngine::TransmitMessage(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CYell * pMsg)
{
	if (m_Summary.IsWorking() && pMsg->IncreaseForwardNumbers() > 0)
	{
		m_Summary.m_RecentData.m_EngineRecords[SS_TOTLE_YELL] += 1;
	}
	++m_nMsgId;
	ASSERT(pMsg != NULL);
	CTransmitionRecord newRecord(pFrom, pTo, pMsg, m_nMsgId);
	StartJudgeProcess(newRecord, 3);
	m_nTransmitCount++;
}

void CHostEngine::StartJudgeProcess(const CTransmitionRecord & transmitionData, int nFrom)
{
	if (!m_bEnableMonitor)
	{
		if (transmitionData.IsUnicast())
		{
			StartJudgeUnicastProcess(transmitionData);
			return;
		}
	}

	CMsgNewJudgeItem item;
	item.m_fRadius = transmitionData.m_pFrom->GetCommunicateRadius();
	item.m_fSecondId = GetSimTime();
	item.m_pHost = transmitionData.m_pFrom->GetHost();
	m_TransmitionBroadcast.AddTail(transmitionData);

	const CMsgCntJudgeReceiverReport * rValue;
	if (m_FullJudgeRecord.Lookup(GetSimTime(), rValue))
	{
		if (rValue)
		{
			SendJudgeOkMsg();
		}
		return;
	}

	if (!m_pJudgeMsg)
	{
		m_pJudgeMsg = new CMsgNewSendJudge();
	}
	m_pJudgeMsg->m_Items.AddTail(item);

	if (!m_bCheckingEvents)
	{
		ASSERT(transmitionData.m_pMsg != NULL);
		SendJudgeMsgToThread(m_pJudgeMsg);
		m_pJudgeMsg = NULL;
	}

}

void CHostEngine::StartJudgeUnicastProcess(const CTransmitionRecord & transmitionData)
{
	double  fSecondId = GetSimTime();
	double fDistanceMax = transmitionData.m_pFrom->GetCommunicateRadius();
	CDoublePoint centerPosition = transmitionData.m_pFrom->GetHost()->m_schedule.GetPosition(fSecondId);
	CDoublePoint targetPosition = transmitionData.m_pTo->GetHost()->m_schedule.GetPosition(fSecondId);
	double fDistance = CDoublePoint::GetDistance(targetPosition, centerPosition);
	CMsgCntJudgeReceiverReport * pRet = new CMsgCntJudgeReceiverReport();
	CReceiverReportItem reportItem;
	reportItem.m_bUnicast = TRUE;
	if (fDistance < fDistanceMax)
	{
		m_TransmitionUnicast.AddTail(transmitionData);
		if (!m_bJudgingUnicast)
		{
			SendJudgeOkMsg();
		}
	}
}

void CHostEngine::RegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds)
{
	RegisterTimer(nCommandId, pUser, lnLaterMilliseconds, FALSE);
	if (pUser)
	{
		SIM_TIME increase = lnLaterMilliseconds;
		SIM_TIME tmpTime = GetSimTime() + increase;
		PreJudgeAllHosts(tmpTime);
	}

	SendEventChangeMsg();
}

void CHostEngine::RegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds, BOOL bStack)
{
	if (lnLaterMilliseconds <= 0)
	{
		ASSERT(0);
		return;
	}
	CEngineEvent newEvent;
	newEvent.m_lnSimTime = GetSimTime() + lnLaterMilliseconds;
	newEvent.m_nCommandId = nCommandId;
	newEvent.m_pUser = pUser;
	newEvent.m_bForecastStack = bStack;
	POSITION pos = m_EventList.GetTailPosition(), lastPos;
	while (pos)
	{
		lastPos = pos;
		CEngineEvent tmp = m_EventList.GetPrev(pos);
		if (tmp.m_lnSimTime <= newEvent.m_lnSimTime)
		{
			m_EventList.InsertAfter(lastPos, newEvent);
			return;
		}
	}
	m_EventList.AddHead(newEvent);
}

SIM_TIME CHostEngine::GetSimTime() const
{
	return m_lnSimTimeMillisecond;
}

void CHostEngine::WriteLog(const CString & strLog)
{
	CString * pNewLog = new CString(strLog);
	m_pMapGui->PostMessage(MSG_ID_WRITE_LOG, 0, (LPARAM)pNewLog);
}

int CHostEngine::GetSpeed() const
{
	return m_lnExpectSimMillisecPerActSec / 1000.0 + 0.5;
}

int CHostEngine::GetActualSpeed() const
{
	SIM_TIME lnExpectPass = GetBoundary() - m_lnSimTimeTickCountStart;
	SIM_TIME lnActualPass = m_lnSimTimeMillisecond - m_lnSimTimeTickCountStart;
	return m_lnExpectSimMillisecPerActSec * lnActualPass / (lnExpectPass*1000.0) + 0.5;
}

CRoadNet * CHostEngine::GetRoadNet()
{
	return m_pRoadNet;
}

void CHostEngine::RegisterUser(CEngineUser * pUser)
{
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		CEngineUser * pTmp = m_NotifyList.GetNext(pos);
		if (pTmp == pUser)
		{
			return;
		}
	}
	m_NotifyList.AddTail(pUser);
}

void CHostEngine::ChangeSummary()
{
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->ChangeSummary(m_Summary);
	}
}

CStatisticSummary & CHostEngine::GetSummary()
{
	return m_Summary;
}

void CHostEngine::OnEveryPeriod()
{
	PeriodForcastAndJudge();

	NotifyConnections();
#ifdef DEBUG
	ULONGLONG ullRecentTickCount = GetTickCount64();
	if (ullRecentTickCount - m_ullLastUiTick > 50)
	{
		m_ullLastUiTick = ullRecentTickCount;
		RefreshUi();
	}
#endif
	SIM_TIME lnCurrentTime = GetSimTime();
	DeleteForecastsBefore(lnCurrentTime);

	SIM_TIME lnInterval = GetDetectConnectInterval();
	RegisterTimer(0, NULL, lnInterval);
}

void CHostEngine::RefreshUi()
{
	CDoublePoint lt, rb;
	m_pMapGui->GetRefreshArea(lt, rb);
	CArray<CHostGui> * pMessageDirectly = new CArray<CHostGui>();
	CArray<CHostGui> * pMessage = NULL;

	RefreshUiDirectly(pMessageDirectly, lt, rb);
	pMessage = pMessageDirectly;

	if (m_pMapGui && IsWindow(m_pMapGui->GetSafeHwnd()))
	{
		if (pMessage->GetSize() > 0)
		{
			m_pMapGui->PostMessage(MSG_ID_MAP_UPDATE_HOSTS, (WPARAM)pMessage, 0);
		}
		else
		{
			delete pMessage;
		}
	}
	else
	{
		delete pMessage;
	}
}

void CHostEngine::RefreshUiDirectly(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb)
{
	CHostGui newHostPosition;
	pMessage->RemoveAll();
	for (int i = 0; i < m_pRoadNet->m_allHosts.GetSize(); ++i)
	{
		if (!m_pRoadNet->m_allHosts[i]->m_schedule.IsScheduleAvailable())
		{
			continue;
		}
		newHostPosition.m_Position = m_pRoadNet->m_allHosts[i]->GetPosition(m_lnSimTimeMillisecond);
		if (newHostPosition.m_Position.m_X < lt.m_X || newHostPosition.m_Position.m_X > rb.m_X)
			continue;
		if (newHostPosition.m_Position.m_Y < lt.m_Y || newHostPosition.m_Position.m_Y > rb.m_Y)
			continue;
		newHostPosition.m_pHost = m_pRoadNet->m_allHosts[i];
		pMessage->Add(newHostPosition);
	}
}

int CHostEngine::GetSurringNodesCount(CDoublePoint currentLocation, double fRadius)
{
	SIM_TIME current = GetSimTime();
	int nRet = 0;
	int nHostCount = m_pRoadNet->m_allHosts.GetSize();
	for (int i = SERVER_NODE_COUNT; i < nHostCount; ++i)
	{
		CDoublePoint dp = m_pRoadNet->m_allHosts[i]->GetPosition(current);
		double fDistance = CDoublePoint::GetDistance(currentLocation, dp);
		if (fDistance <= fRadius)
		{
			nRet++;
		}
	}
	return nRet;
}

CMsgCntJudgeReceiverReport * CHostEngine::GetUnicastReport(const CTransmitionRecord & tr)
{
	double  fSecondId = GetSimTime();
	double fDistanceMax = tr.m_pFrom->GetCommunicateRadius();
	CDoublePoint centerPosition = tr.m_pFrom->GetHost()->m_schedule.GetPosition(fSecondId);
	CDoublePoint targetPosition = tr.m_pTo->GetHost()->m_schedule.GetPosition(fSecondId);
	double fDistance = CDoublePoint::GetDistance(targetPosition, centerPosition);
	CMsgCntJudgeReceiverReport * pRet = new CMsgCntJudgeReceiverReport();
	CReceiverReportItem reportItem;
	reportItem.m_bUnicast = TRUE;
	if (fDistance < fDistanceMax)
	{
		CHostGui hg;
		hg.m_pHost = tr.m_pTo->GetHost();
		hg.m_Position = targetPosition;
		reportItem.m_Hosts.AddTail(hg);
		pRet->m_Items.SetAt(tr.m_pFrom->GetHost(), reportItem);
	}
	return pRet;
}

SIM_TIME CHostEngine::GetActualSimMillisecPerActSec()
{
	return 1.0*m_lnExpectSimMillisecPerActSec * m_lnEventCheckBoundary / m_lnEventCheckBoundaryExpect;
}

SIM_TIME CHostEngine::GetBoundary() const
{
	ULONGLONG ulCurrentTK = GetTickCount64();
	ULONGLONG ulDiffer = ulCurrentTK - m_ulStartTickCount;
	SIM_TIME lnRet = m_lnExpectSimMillisecPerActSec * ulDiffer / 1000 + m_lnSimTimeTickCountStart;
	return lnRet;
}

void CHostEngine::NotifyTimeChange()
{
	if (abs(m_ulLastNotifyTime - m_lnSimTimeMillisecond) < GetActualSpeed() * 1000)
	{
		return;
	}
	m_ulLastNotifyTime = m_lnSimTimeMillisecond;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineTimeChanged(m_lnSimTimeMillisecond);
	}
	ChangeSummary();
}

void CHostEngine::UpdateStartTick()
{
	m_lnSimTimeTickCountStart = m_lnSimTimeMillisecond;
	m_ulStartTickCount = GetTickCount64();
}

// double CHostEngine::GetAveSurroundingHosts(double fRadius, int nComment)
// {
// 	double fTotalHosts = 0;
// 	int nAnonyEndCount = 0;
// 	
// 	int rKey;
// 	CRoutingStatisticsInfo * rValue;
// 	POSITION posMsg = m_SendingMsgs.GetStartPosition();
// 
// 	while (posMsg)
// 	{
// 		m_SendingMsgs.GetNextAssoc(posMsg, rKey, rValue);
// 		POSITION findResult = rValue->GetHopInfo().GetRecordWith(nComment);
// 		if (findResult)
// 		{
// 			++nAnonyEndCount;
// 			const CMsgHopInfo & findInfo = rValue->GetHopInfo().GetRecordAt(findResult);
// 			int nHostNumber = m_pRoadNet->GetHostNumberInRange(findInfo.m_Location, fRadius, findInfo.m_Time);
// 			fTotalHosts += nHostNumber;
// 		}
// 	}
// 	double fRet = fTotalHosts / nAnonyEndCount;
// 	return fRet;
// }
// 
// int CHostEngine::GetSourceOnRing(double fInside, double fOutside)
// {
// 	int nRet = 0;
// 
// 	int rKey;
// 	CRoutingStatisticsInfo * rValue;
// 	POSITION posMsg = m_SendingMsgs.GetStartPosition();
// 
// 	while (posMsg)
// 	{
// 		m_SendingMsgs.GetNextAssoc(posMsg, rKey, rValue);
// 		if (rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].nCount == 1)
// 		{
// 			ASSERT(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].nCount == 1);
// 			ASSERT(rValue->m_CnP[MSG_HOP_STATE_SOURCE].nCount == 1);
// 			const CMsgInsideInfo & cntInfo = rValue->GetHopInfo();
// 			CHost * pOriginalRequester = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_SOURCE].pos).m_pProtocol->GetHost();
// 			CDoublePoint dpAnonymityEnd = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].pos).m_Location;
// 			SIM_TIME lnEndTime = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].pos).m_Time;
// 			CDoublePoint dpReqerLocation = pOriginalRequester->GetPosition(lnEndTime);
// 			double fDistance = CDoublePoint::GetDistance(
// 				dpReqerLocation,
// 				dpAnonymityEnd
// 			);
// 			if (fDistance >= fInside && fDistance <= fOutside)
// 			{
// 				nRet++;
// 			}
// 		}
// 	}
// 	return nRet;
// }

CHost * CHostEngine::GetHost(int nHostId) const
{
	return m_pRoadNet->m_allHosts[nHostId];
}

int CHostEngine::CheckEventList()
{
	static int nRunTimes = 0;
	static int nWaitTimes = 0;
	static int nPeriod = 0;

	++nRunTimes;
	
	double x = nWaitTimes * 1.0 / nRunTimes;

	if (GetSimTime() > 2000000)
	{
		//ASSERT(0);
	}
	if (m_bPaused)
	{
		SendEventChangeMsg();
		return 0;
	}
	int nUnicast = m_TransmitionUnicast.GetSize();
	int nBroadcast = m_TransmitionBroadcast.GetSize();
	int nSizeEvent = m_EventList.GetSize();
	ASSERT(nSizeEvent < 150);
	if (nUnicast == 0 && nBroadcast == 0)
	{
		ASSERT(m_EventList.GetSize() < 2);
		if (!m_EventList.IsEmpty())
		{
			CEngineEvent FirstEvent = m_EventList.GetHead();
			SIM_TIME lnBoundary = GetBoundary();
			if (m_blimitedSpeed && FirstEvent.m_lnSimTime > lnBoundary)
			{
				SendEventChangeMsg();
				return 0;
			}
			if (m_lnSimTimeMillisecond != FirstEvent.m_lnSimTime)
			{
				if (!DeletePreFullJudge(m_lnSimTimeMillisecond))
				{
					nWaitTimes++;
					SendEventChangeMsg();
					return 0;
				}
				if (!GetFullJudgeReport(FirstEvent.m_lnSimTime))
				{
					SendEventChangeMsg();
					return 0;
				}
				NotifyTimeChange();
				m_Summary.AddTag(m_lnSimTimeMillisecond);
				m_lnSimTimeMillisecond = FirstEvent.m_lnSimTime;
			}

			do 
			{
				CEngineEvent HeadEvent = m_EventList.GetHead();
				if (HeadEvent.m_lnSimTime > FirstEvent.m_lnSimTime)
				{
					break;
				}
				m_EventList.RemoveHead();

				if (HeadEvent.m_pUser)
				{
					HeadEvent.m_pUser->OnEngineTimer(HeadEvent.m_nCommandId);
				}
				else
				{
					if (!HeadEvent.m_bForecastStack)
					{
						++nPeriod;
						OnEveryPeriod();
					}
				}
			} while (m_EventList.GetSize()>0);
		}
	}
	return 0;
}

void CHostEngine::OnCommonTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_PERIOD)
	{
		if (!m_bFinishFirstForecast)
		{
			return;
		}
		if (!m_bPaused)
		{
			SIM_TIME Interval = GetPeriodDefaultInterval();
			m_lnEventCheckBoundaryExpect += Interval;
			if (m_lnEventCheckBoundary - m_lnSimTimeMillisecond < Interval)
			{
				m_lnEventCheckBoundary += Interval;
				if (m_bWaitingActualTime)
				{
					m_bWaitingActualTime = FALSE;
					PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
				}
				m_nTransmitCount = 0;
			}
		}
	}
}

void CHostEngine::OnStartEngine(WPARAM wParam, LPARAM lParam)
{
	OnResetEngine(0, 0);
	m_bPaused = false;
	m_bFinishFirstForecast = FALSE;
	SendForecastCommand(0);
	m_ullLastUiTick = GetTickCount64();
	//UINT_PTR ret = SetCommonTimer(TIMER_ID_PERIOD, TIMER_LONG_PERIOD, TRUE);
}

void CHostEngine::OnResetEngine(WPARAM wParam, LPARAM lParam)
{
	m_ulStartTickCount = GetTickCount64();
	KillCommonTimer(TIMER_ID_PERIOD);
	WriteLog(_T("RESET"));
	m_bPaused = true;
	m_lnSimTimeMillisecond = 0;
	NotifyTimeChange();
	m_lnSimTimeTickCountStart = m_lnSimTimeMillisecond;
	m_lnEventCheckBoundary = GetPeriodDefaultInterval();
	m_pForecastThread->PostThreadMessage(MSG_ID_POS_FRCST_REMOVEALL, 0, 0);
	m_Connections.Reset(m_pRoadNet->m_allHosts.GetSize());
}

void CHostEngine::OnPauseEngine(WPARAM wParam, LPARAM lParam)
{
	WriteLog(_T("PAUSE"));
	m_bPaused = true;
}

void CHostEngine::OnResumeEngine(WPARAM wParam, LPARAM lParam)
{
	WriteLog(_T("RESUME"));
	if (m_bPaused)
	{
		m_bPaused = false;
		UpdateStartTick();
	}
}

void CHostEngine::OnFinishedOneForecast(WPARAM wParam, LPARAM lParam)
{
	SIM_TIME * pMsg = (SIM_TIME *)wParam;
// 	SIM_TIME lnDiffer = GetForecastThreshhold(1);
// 	SIM_TIME lnCurrentTime = GetSimTime();
// 
// 	SIM_TIME lnNxtForecastSimTime = lnCurrentTime + lnDiffer;
// 	BOOL bRet = SendForecastCommand(lnNxtForecastSimTime);

	m_lnLastForecastedSimTime = *pMsg;
	delete pMsg;
	pMsg = NULL;
	if (m_bFinishFirstForecast == FALSE)
	{
		OnFirstForecastFinished();
	}
}

void CHostEngine::OnFinishedForecastOnce(SIM_TIME lnSimTime)
{
	SIM_TIME * pMsg = new SIM_TIME(lnSimTime);
	PostThreadMessage(MSG_ID_ENGINE_FORECAST_OK, (WPARAM)pMsg, 0);
}

BOOL CHostEngine::SendForecastCommand(SIM_TIME lnSimTime)
{
	if (lnSimTime <= m_lnLastForecastSimTime + GetForecastThreshhold(0))
	{
		return FALSE;
	}

	CMsgPosFrcstNewTime * pForecastMsg = new CMsgPosFrcstNewTime();
	pForecastMsg->m_lnSimTime = lnSimTime;
	m_pForecastThread->PostThreadMessage(MSG_ID_POS_FRCST_NEW_TIME, (WPARAM)pForecastMsg, 0);
	m_lnLastForecastSimTime = pForecastMsg->m_lnSimTime;
	return TRUE;
}

void CHostEngine::DeleteForecastsBefore(int nBeforeSecond)
{
	m_pForecastThread->PostThreadMessage(MSG_ID_POS_FRCST_COMPLETE, (WPARAM)nBeforeSecond, 0);
}

SIM_TIME CHostEngine::GetForecastThreshhold(int nExtraBlock) const
{
	int nBlockCount;
	SIM_TIME lnPredictTime;
	SIM_TIME lnHalfBlockTime;
	m_pRoadNet->CalculateBlockAndPredictTime(nBlockCount, lnPredictTime, lnHalfBlockTime, m_fCommunicationRadius);
	return lnPredictTime + lnHalfBlockTime * nExtraBlock;
}

SIM_TIME CHostEngine::GetPeriodDefaultInterval()
{
	return TIMER_LONG_PERIOD * m_lnExpectSimMillisecPerActSec / 1000.0;
}

void CHostEngine::JudgeAllUnicastOk()
{
	m_bJudgingUnicast = TRUE;
	while (m_TransmitionUnicast.GetSize() > 0)
	{
		CTransmitionRecord head = m_TransmitionUnicast.GetHead();
		m_TransmitionUnicast.RemoveHead();
		head.m_pTo->GetHost()->OnHearMsg(head.m_pMsg);
		delete head.m_pMsg;
	}
	m_bJudgingUnicast = FALSE;
}

void CHostEngine::JudgeAllFullForcast()
{
	const CMsgCntJudgeReceiverReport * pMsg = NULL;
	if (m_FullJudgeRecord.Lookup(GetSimTime(), pMsg))
	{
		if (pMsg == NULL)
		{
			return;
		}
		else
		{
			POSITION pos = m_TransmitionBroadcast.GetHeadPosition(), posLast;
			while (pos)
			{
				int nSize = m_TransmitionBroadcast.GetSize();
				int nsss = m_FullJudgeRecord.GetSize();
				
				posLast = pos;
				CTransmitionRecord rValue = m_TransmitionBroadcast.GetNext(pos);

				CReceiverReportItem reportItem;
				if (!pMsg->m_Items.Lookup(rValue.m_pFrom->GetHost(), reportItem))
				{
					continue;
				}
				ASSERT(rValue.m_pMsg != NULL);
				POSITION posReport = reportItem.m_Hosts.GetHeadPosition();
				while (posReport)
				{
					CHost * pHost = reportItem.m_Hosts.GetNext(posReport).m_pHost;
					pHost->OnHearMsg(rValue.m_pMsg);
				}
				delete rValue.m_pMsg;
				m_TransmitionBroadcast.RemoveAt(posLast);
			}

			int nSrrize = m_TransmitionBroadcast.GetSize();
			int nrrsss = m_FullJudgeRecord.GetSize();
			ASSERT(nSrrize == 0);
		}
	}
	else
	{
		return;
	}
}

void CHostEngine::JudgeAllBroadcast(const CMsgCntJudgeReceiverReport * pMsg)
{
	if (pMsg->m_bFullReport)
	{
		m_FullJudgeRecord[pMsg->m_lnTime] = pMsg;
		return;
	}
	POSITION pos = m_TransmitionBroadcast.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CTransmitionRecord rValue = m_TransmitionBroadcast.GetNext(pos);

		CReceiverReportItem reportItem;
		if (!pMsg->m_Items.Lookup(rValue.m_pFrom->GetHost(), reportItem))
		{
			continue;
		}
		ASSERT(rValue.m_pMsg != NULL);
		POSITION posReport = reportItem.m_Hosts.GetHeadPosition();
		while (posReport)
		{
			CHost * pHost = reportItem.m_Hosts.GetNext(posReport).m_pHost;
			//pHost->OnHearMsg(rValue.m_pMsg);
		}
		delete rValue.m_pMsg;
		m_TransmitionBroadcast.RemoveAt(posLast);
	}
}

BOOL CHostEngine::PreJudgeAllHosts(SIM_TIME lnTime)
{
	const CMsgCntJudgeReceiverReport * value;
	if (m_FullJudgeRecord.Lookup(lnTime, value))
	{
		return FALSE;
	}
	else
	{
		CMsgNewSendJudge * pJudgeMsg = new CMsgNewSendJudge();
		pJudgeMsg->m_bFullJudge = TRUE;
		m_FullJudgeRecord[lnTime] = NULL;
		CMsgNewJudgeItem item;
		item.m_fRadius = m_fCommunicationRadius;
		item.m_fSecondId = lnTime;

		int nHostCount = m_pRoadNet->m_allHosts.GetSize();
		for (int i = 0; i < nHostCount; ++i)
		{
			CHost * pHost = m_pRoadNet->m_allHosts.GetAt(i);
			item.m_pHost = pHost;
			pJudgeMsg->m_Items.AddTail(item);
		}
		SendJudgeMsgToThread(pJudgeMsg);
		return TRUE;
	}
}

BOOL CHostEngine::DeletePreFullJudge(SIM_TIME lnTime)
{
	int nRecordLength = m_FullJudgeRecord.GetSize();
	//ASSERT(nRecordLength < 10 * m_nJudgeMax);
	const CMsgCntJudgeReceiverReport * pReport = NULL;
	if (m_FullJudgeRecord.Lookup(lnTime, pReport))
	{
		if (pReport == NULL)
		{
			return FALSE;
		}
		delete pReport;
		m_FullJudgeRecord.RemoveKey(lnTime);
	}
	return TRUE;

}

void CHostEngine::SendEventChangeMsg()
{
	if (m_nMsgCount == 0)
	{
		m_nMsgCount++;
		PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
	}
}

void CHostEngine::SendJudgeOkMsg()
{
	if (m_nJudgeOkMsgCount == 0)
	{
		m_nJudgeOkMsgCount++;
		PostThreadMessage(MSG_ID_ENGINE_JUDGE_OK, 0, 1);
	}
}

void CHostEngine::SendJudgeMsgToThread(CMsgNewSendJudge * pJudgeMsg)
{
	++m_nBusyJudgeThreadCount;
	CConnectionJudge * pJudge = m_JudgeList[m_nJudgeRunTime++%m_nJudgeMax];
	pJudge->PostThreadMessage(MSG_ID_JUDGE_NEW_SEND, (WPARAM)pJudgeMsg, 0);
}

BOOL CHostEngine::NotifyConnections()
{
	const CMsgCntJudgeReceiverReport * pMsg = GetFullJudgeReport(GetSimTime());
	if (!pMsg)
	{
		return FALSE;
	}
	POSITION pos = pMsg->m_Items.GetStartPosition();
	while (pos)
	{
		CHost * rKey;
		CReceiverReportItem rValue;
		pMsg->m_Items.GetNextAssoc(pos, rKey, rValue);
		if (rValue.m_Hosts.GetSize() > 1)
		{
			rKey->OnConnection(rValue.m_Hosts);
		}
	}
	return TRUE;
}

SIM_TIME CHostEngine::GetDetectConnectInterval()
{
	SIM_TIME lnSearchInterval = 1000 * m_fCommunicationRadius / (2 * m_pRoadNet->GetSpeedLimit());
	return lnSearchInterval;
}

const CMsgCntJudgeReceiverReport * CHostEngine::GetFullJudgeReport(SIM_TIME lnTime)
{
	const CMsgCntJudgeReceiverReport * pMsg = NULL;
	if (m_FullJudgeRecord.Lookup(lnTime, pMsg))
	{
		return pMsg;
	}
	else
	{
		return NULL;
	}
}

void CHostEngine::PreJudgeSeveralPeriods(SIM_TIME lnInterval)
{
	SIM_TIME lnCurrent = GetSimTime();
	for (int i = 0; i <= m_nJudgeMax; ++i)
	{
		PreJudgeAllHosts(lnCurrent + i * lnInterval);
	}
}

void CHostEngine::PeriodForcastAndJudge()
{
	SIM_TIME lnCurrentTime = GetSimTime();

	SIM_TIME lnInterval = GetDetectConnectInterval();

	for (int i = 0; i < 30; ++i)
	{
		SendForecastCommand(lnCurrentTime + i * lnInterval);
	}

	PreJudgeSeveralPeriods(lnInterval);
}

void CHostEngine::OnJudgeOk(WPARAM wParam, LPARAM lParam)
{
	if (!wParam)
	{
		--m_nJudgeOkMsgCount;
	}
	else
	{
		--m_nBusyJudgeThreadCount;
		ASSERT(m_nBusyJudgeThreadCount >= 0);
	}
	JudgeAllUnicastOk();
	CMsgCntJudgeReceiverReport * pMsg = (CMsgCntJudgeReceiverReport*)wParam;
	if (pMsg)
	{
		//m_Connections.UpdateByJudgeReport(pMsg);
		JudgeAllBroadcast(pMsg);
		if (!pMsg->m_bFullReport)
		{
			delete pMsg;
		}
	}
	JudgeAllFullForcast();

	int nUnicast = m_TransmitionUnicast.GetSize();
	int nBroadcast = m_TransmitionBroadcast.GetSize();
	if (nUnicast == 0 && nBroadcast == 0)
	{
		SendEventChangeMsg();
	}
}

void CHostEngine::OnEventListChanged(WPARAM wParam, LPARAM lParam)
{
	--m_nMsgCount;
	int nUnicast = m_TransmitionUnicast.GetSize();
	int nBroadcast = m_TransmitionBroadcast.GetSize();
	m_bCheckingEvents = TRUE;
	CheckEventList();
	//SendEventChangeMsg();
	if (m_pJudgeMsg)
	{
		SendJudgeMsgToThread(m_pJudgeMsg);
		m_pJudgeMsg = NULL;
	}
	m_bCheckingEvents = FALSE;
}

void CHostEngine::IncreaseSpeed(WPARAM wParam, LPARAM lParam)
{
	UpdateStartTick();
	m_lnExpectSimMillisecPerActSec *= 1.5;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineSpeedChanged();
	}
}

void CHostEngine::DecreaseSpeed(WPARAM wParam, LPARAM lParam)
{
	UpdateStartTick();
	m_lnExpectSimMillisecPerActSec /= 1.5;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineSpeedChanged();
	}
}

void CHostEngine::WatchTime(WPARAM wParam, LPARAM lParam)
{
	PostThreadMessage(MSG_ID_ENGINE_WATCH_TIME, 0, 0);
}

void CHostEngine::OnFirstForecastFinished()
{
	// TURN ON ALL HOST'S PROTOCOL
	PeriodForcastAndJudge();

	SIM_TIME lnInterval = GetDetectConnectInterval();
	RegisterTimer(0, NULL, lnInterval);
	int nHostCount = m_pRoadNet->m_allHosts.GetSize();
	for (int i = 0; i < nHostCount; ++i)
	{
		CHost * pHost = m_pRoadNet->m_allHosts.GetAt(i);
		if (pHost && pHost->m_pProtocol)
		{
			pHost->m_pProtocol->Turn(TRUE);
		}
	}
	m_bFinishFirstForecast = TRUE;
}

BEGIN_MESSAGE_MAP(CHostEngine, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_START, OnStartEngine)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_RESET, OnResetEngine)
 	ON_THREAD_MESSAGE(MSG_ID_ENGINE_PAUSE, OnPauseEngine)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_RESUME, OnResumeEngine)
 	ON_THREAD_MESSAGE(MSG_ID_ENGINE_FORECAST_OK, OnFinishedOneForecast)
 	ON_THREAD_MESSAGE(MSG_ID_ENGINE_JUDGE_OK, OnJudgeOk)
 	ON_THREAD_MESSAGE(MSG_ID_ENGINE_EVENT_CHANGED, OnEventListChanged)
 	ON_THREAD_MESSAGE(MSG_ID_ENGINE_SPEED_UP, IncreaseSpeed)
 	ON_THREAD_MESSAGE(MSG_ID_ENGINE_SPEED_DOWN, DecreaseSpeed)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_WATCH_TIME, WatchTime)
END_MESSAGE_MAP()


// CHostEngine 消息处理程序
