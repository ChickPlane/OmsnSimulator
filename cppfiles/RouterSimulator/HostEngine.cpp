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
	, m_nJudgeMax(1)
	, m_bEnableMonitor(TRUE)
	, m_blimitedSpeed(TRUE)
	, m_bWaitingActualTime(TRUE)
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
}

void CHostEngine::BreakMapGui()
{
	m_pMapGui = NULL;
}

void CHostEngine::TransmitMessage(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CYell * pMsg)
{
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
		if (transmitionData.m_pTo != NULL)
		{
			CMsgCntJudgeReceiverReport * pReport = GetUnicastReport(transmitionData);
			PostThreadMessage(MSG_ID_ENGINE_JUDGE_OK, (WPARAM)pReport, 0);
			m_TransmitionMap[transmitionData.m_nMsgId] = transmitionData;
			return;
		}
	}

	static int nRuntimes = 0;
	nRuntimes++;
	ASSERT(transmitionData.m_pMsg != NULL);
	CMsgNewSendJudge * pJudgeMsg = new CMsgNewSendJudge();
	pJudgeMsg->m_fRadius = transmitionData.m_pFrom->GetCommunicateRadius();
	pJudgeMsg->m_fSecondId = GetSimTime();
	pJudgeMsg->m_pHost = transmitionData.m_pFrom->GetHost();
	pJudgeMsg->m_nMsgId = transmitionData.m_nMsgId;
	m_TransmitionMap[pJudgeMsg->m_nMsgId] = transmitionData;

	CConnectionJudge * pJudge = m_JudgeList[nRuntimes%m_nJudgeMax];
	pJudge->PostThreadMessage(MSG_ID_JUDGE_NEW_SEND, (WPARAM)pJudgeMsg, nFrom);
}

void CHostEngine::RegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds)
{
	CEngineEvent newEvent;
	newEvent.m_lnSimTime = GetSimTime() + lnLaterMilliseconds;
	newEvent.m_nCommandId = nCommandId;
	newEvent.m_pUser = pUser;
	POSITION pos = m_EventList.GetTailPosition(), lastPos;
	while (pos)
	{
		lastPos = pos;
		CEngineEvent tmp = m_EventList.GetPrev(pos);
		if (tmp.m_lnSimTime <= newEvent.m_lnSimTime)
		{
			m_EventList.InsertAfter(lastPos, newEvent);
			PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
			return;
		}
	}
	m_EventList.AddHead(newEvent);
	PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
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

void CHostEngine::ChangeSummary(const CStatisticSummary & summary)
{
	m_Summary = summary;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->ChangeSummary(summary);
	}
}

CStatisticSummary & CHostEngine::GetSummary()
{
	return m_Summary;
}

void CHostEngine::OnEveryPeriod()
{
	SIM_TIME lnCurrentTime = GetSimTime();
	SendForecastCommand(lnCurrentTime);
	RefreshUi();
	DeleteForecastsBefore(lnCurrentTime);

	SIM_TIME Interval = GetPeriodDefaultInterval();
	RegisterTimer(0, NULL, Interval);

// 	int nHostCount = m_pRoadNet->m_allHosts.GetSize();
// 	int nSum = 0;
// 	for (int i = 1; i < nHostCount; ++i)
// 	{
// 		nSum += m_pRoadNet->m_allHosts[i]->m_pProtocol->GetDebugNumber(0);
// 	}
	CString strOut;
	strOut.Format(_T("\n%d\t%d"), lnCurrentTime, GetTickCount64() - m_ulStartTickCount);
	OutputDebugString(strOut);
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

bool CHostEngine::RefreshUiOptimize(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb)
{
	SIM_TIME lnPeriodDefaultInterval = INT_MAX;
	CHostGui newHostPosition;
	pMessage->RemoveAll();

	CMsgPosFrcstReport * pReport = m_pForecastThread->GetReport(m_lnSimTimeMillisecond, lnPeriodDefaultInterval);
	if (pReport)
	{
		CDoublePoint recentRange;
		double fMaxMove = (m_lnSimTimeMillisecond - pReport->m_lnSimTime) * m_pRoadNet->GetSpeedLimit() / 1000.0;
		recentRange.m_X = lt.m_X - fMaxMove;
		while (recentRange.m_X < rb.m_X + fMaxMove)
		{
			recentRange.m_Y = lt.m_Y - fMaxMove;
			while (recentRange.m_Y < rb.m_Y + fMaxMove)
			{
				if (!m_pRoadNet->HasHash(recentRange))
				{
					recentRange.m_Y += m_pRoadNet->GetHashInterval();
					continue;
				}
				DWORD HashValue = m_pRoadNet->GetHashValue(&recentRange);
				CHostReference tmpReference;
				if (!pReport->m_Reference.Lookup(HashValue, tmpReference))
				{
					recentRange.m_Y += m_pRoadNet->GetHashInterval();
					continue;
				}
				POSITION pos = tmpReference.m_Hosts.GetHeadPosition();
				while (pos)
				{
					CHostGui tmpGui = tmpReference.m_Hosts.GetNext(pos);
					newHostPosition.m_Position = tmpGui.m_pHost->GetPosition(m_lnSimTimeMillisecond);

					if (newHostPosition.m_Position.m_X < lt.m_X || newHostPosition.m_Position.m_X > rb.m_X)
						continue;
					if (newHostPosition.m_Position.m_Y < lt.m_Y || newHostPosition.m_Position.m_Y > rb.m_Y)
						continue;

					pMessage->Add(tmpGui);
				}
				recentRange.m_Y += m_pRoadNet->GetHashInterval();
			}
			recentRange.m_X += m_pRoadNet->GetHashInterval();
		}
		return true;
	}
	return false;
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
	pRet->m_nMsgId = tr.m_nMsgId;
	if (fDistance < fDistanceMax)
	{
		CHostGui hg;
		hg.m_pHost = tr.m_pTo->GetHost();
		hg.m_Position = targetPosition;
		pRet->m_Hosts.AddTail(hg);
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
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineTimeChanged(m_lnSimTimeMillisecond);
	}
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

VOID CALLBACK CHostEngine::timerFunEE(HWND wnd, UINT msg, UINT_PTR id, DWORD d)
{
	int a = msg;
}

int CHostEngine::CheckEventList()
{
	if (m_bPaused)
	{
		PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
		return 0;
	}
	if (m_TransmitionMap.IsEmpty())
	{
		if (!m_EventList.IsEmpty())
		{
			CEngineEvent FirstEvent = m_EventList.GetHead();
			SIM_TIME lnBoundary = GetBoundary();
			if (m_blimitedSpeed && FirstEvent.m_lnSimTime > lnBoundary)
			{
				PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
				return 0;
			}
			m_EventList.RemoveHead();

			if (m_lnSimTimeMillisecond != FirstEvent.m_lnSimTime)
			{
				NotifyTimeChange();
			}
			m_lnSimTimeMillisecond = FirstEvent.m_lnSimTime;
			if (FirstEvent.m_pUser)
			{
				m_lnSimTimeMillisecond = FirstEvent.m_lnSimTime;
				FirstEvent.m_pUser->OnEngineTimer(FirstEvent.m_nCommandId);
			}
			else
			{
				OnEveryPeriod();
			}
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
		m_lnSimTimeTickCountStart = m_lnSimTimeMillisecond;
		m_ulStartTickCount = GetTickCount64();
	}
}

void CHostEngine::OnFinishedOneForecast(WPARAM wParam, LPARAM lParam)
{
	SIM_TIME * pMsg = (SIM_TIME *)wParam;
	SIM_TIME lnDiffer = GetForecastThreshhold(1);
	SIM_TIME lnCurrentTime = GetSimTime();

	SIM_TIME lnNxtForecastSimTime = lnCurrentTime + lnDiffer;
	BOOL bRet = SendForecastCommand(lnNxtForecastSimTime);
// 
// 	if (lnNxtForecastSimTime <= m_lnEventCheckBoundary || !m_blimitedSpeed)
// 	{
// 		BOOL bRet = SendForecastCommand(lnNxtForecastSimTime);
// 		ASSERT(bRet == TRUE);
// 	}
// 	else
// 	{
// 		BOOL bRet = SendForecastCommand(m_lnEventCheckBoundary);
// 		ASSERT(bRet == TRUE);
// 	}
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

void CHostEngine::OnJudgeOk(WPARAM wParam, LPARAM lParam)
{
	CMsgCntJudgeReceiverReport * pMsg = (CMsgCntJudgeReceiverReport*)wParam;
	CTransmitionRecord judgedRecord = m_TransmitionMap[pMsg->m_nMsgId];
	ASSERT(judgedRecord.m_pMsg != NULL);
	m_TransmitionMap.RemoveKey(pMsg->m_nMsgId);
	POSITION pos = pMsg->m_Hosts.GetHeadPosition();
	while (pos)
	{
		CHostGui receivedHost = pMsg->m_Hosts.GetNext(pos);
		ASSERT(judgedRecord.m_pMsg != NULL);
		receivedHost.m_pHost->OnHearMsg(judgedRecord.m_pMsg);
	}
	delete judgedRecord.m_pMsg;
	delete pMsg;
	if (m_TransmitionMap.IsEmpty())
	{
		PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
	}
}

void CHostEngine::OnEventListChanged(WPARAM wParam, LPARAM lParam)
{
	CheckEventList();
}

void CHostEngine::IncreaseSpeed(WPARAM wParam, LPARAM lParam)
{
	m_lnSimTimeTickCountStart = m_lnSimTimeMillisecond;
	m_ulStartTickCount = GetTickCount64();
	m_lnExpectSimMillisecPerActSec *= 1.5;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineSpeedChanged();
	}
}

void CHostEngine::DecreaseSpeed(WPARAM wParam, LPARAM lParam)
{
	m_lnSimTimeTickCountStart = m_lnSimTimeMillisecond;
	m_ulStartTickCount = GetTickCount64();
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
	RegisterTimer(0, NULL, m_lnEventCheckBoundary / 2);
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
