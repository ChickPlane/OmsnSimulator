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



// CHostEngine

IMPLEMENT_DYNCREATE(CHostEngine, CWinThread)

CHostEngine::CHostEngine()
	: m_lnSimTimeMillisecond(0)
	, m_lnSimMillisecondPerActualSecond(15000)
	, m_pRoadNet(NULL)
	, m_pMapGui(NULL)
	, m_bPaused(true)
	, m_bWaitingTimer(false)
	, m_lnLastForecastSimTime(0)
	, m_lnLastForecastedSimTime(INT_MIN)
	, m_nMsgId(0)
	, m_nJudgeMax(15)
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

void CHostEngine::TransmitMessage(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CRoutingMsg * pMsg)
{
	++m_nMsgId;
	ASSERT(pMsg != NULL);
	CTransmitionRecord newRecord(pFrom, pTo, pMsg, m_nMsgId);
	SIM_TIME lnDiffer = GetForecastThreshhold();
	CMsgPosFrcstReport * pReport = m_pForecastThread->GetReport(GetSimTime(), lnDiffer);
	if (pReport && m_TransmitionWaitingList.IsEmpty())
	{
		StartJudgeProcess(newRecord, 1);
	}
	else
	{
		SendForecastCommand(GetSimTime());
		m_TransmitionWaitingList.AddTail(newRecord);
	}
}

void CHostEngine::StartJudgeProcess(const CTransmitionRecord & transmitionData, int nFrom)
{
	static int nRuntimes = 0;
	nRuntimes++;
	ASSERT(transmitionData.m_pMsg != NULL);
	CMsgNewSendJudge * pJudgeMsg = new CMsgNewSendJudge();
	pJudgeMsg->m_fRadius = transmitionData.m_pFrom->GetCommunicateRadius();
	pJudgeMsg->m_fSecondId = GetSimTime();
	pJudgeMsg->m_pHost = transmitionData.m_pFrom->GetHost();
	pJudgeMsg->m_nMsgId = transmitionData.m_nMsgId;
	m_TransmitionMap[pJudgeMsg->m_nMsgId] = transmitionData;
// 	CString strLog;
// 	strLog.Format(_T("Start Judge %d"), pJudgeMsg->m_nMsgId);
// 	OutputDebugString(strLog);
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

int CHostEngine::GetSpeed()
{
	return m_lnSimMillisecondPerActualSecond / 1000.0 + 0.5;
}

void CHostEngine::RecordPackageStateChange(int nDataId, const CMsgInsideInfo & msgInfo, int nState)
{
	int nHopIndex = msgInfo.GetStateIndex(nState);
	ASSERT(nHopIndex != -1);

	const CMsgHopInfo & info = msgInfo.GetTailRecord();
	int nHostId = info.m_pProtocol->GetHostId();
	CString strCommon = GetCommonName(info.m_nComment);
	if (!strCommon.IsEmpty())
	{
		CString strLog;
		strLog.Format(_T("[%6d] H(%3d) %s"), nDataId, nHostId, strCommon.GetBuffer(0));
		WriteLog(strLog);
	}

	CRoutingStatisticsInfo * pLookupResult = NULL;
	if (!m_SendingMsgs.Lookup(nDataId, pLookupResult))
	{
		pLookupResult = new CRoutingStatisticsInfo();
		pLookupResult->SetHopInfo(msgInfo);
		m_SendingMsgs[nDataId] = pLookupResult;
	}
	else
	{
		ASSERT(msgInfo.IsAnExtent(pLookupResult->GetHopInfo()));
		pLookupResult->SetHopInfo(msgInfo);
	}

	ReportMessageStatisticsChanged();
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

void CHostEngine::OnEveryPeriod()
{
	ASSERT(CheckEventList() == 0);
	m_lnSimTimeMillisecond = m_lnEventCheckBoundary;
	m_lnEventCheckBoundary += GetPeriodDefaultInterval();

// 	CString strOut;
// 	strOut.Format(_T("\nRecent %d.%03d"), m_lnSimTimeMillisecond / 1000, m_lnSimTimeMillisecond % 1000);
// 	OutputDebugString(strOut);

	ForecastSeveralPeriod(3);
	RefreshUi();


	ULONGLONG llDrawTime = GetTickCount64();
	m_llLastUpdateTime = llDrawTime;

	CheckEventList();

	SIM_TIME deletetime = GetForecastThreshhold() * 3;
	if (deletetime < GetPeriodDefaultInterval() * 3)
	{
		deletetime = GetPeriodDefaultInterval() * 3;
	}
	DeleteForecastsBefore(deletetime);
}

void CHostEngine::RefreshUi()
{
	CDoublePoint lt, rb;
	m_pMapGui->GetRefreshArea(lt, rb);
	CArray<CHostGui> * pMessageDirectly = new CArray<CHostGui>();
	CArray<CHostGui> * pMessageOptimize = new CArray<CHostGui>();
	CArray<CHostGui> * pMessage = NULL;
	ULONGLONG lla = GetTickCount64();
	bool bRet = RefreshUiOptimize(pMessageOptimize, lt, rb);
	ULONGLONG llb = GetTickCount64();
	ULONGLONG llc = llb - lla;
	if (bRet)
	{
		pMessage = pMessageOptimize;
		delete pMessageDirectly;
		pMessageDirectly = NULL;
	}
	else
	{
		RefreshUiDirectly(pMessageDirectly, lt, rb);
		pMessage = pMessageDirectly;
		delete pMessageOptimize;
		pMessageOptimize = NULL;
	}

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
	SIM_TIME lnPeriodDefaultInterval = GetPeriodDefaultInterval();
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

void CHostEngine::ReportMessageStatisticsChanged()
{
	int StateStatistics[MSG_HOP_STATE_MAX] = { 0 };
	double fTotalAnonyDistance = 0;
	double fTotalObfuscationNum = 0;
	double fMaxAnonyDistance = 0;
	SIM_TIME lnTotalAnonyTime = 0, lnTotalLatency = 0;
	int nInterHopCountTotle = 0;

	int rKey;
	CRoutingStatisticsInfo * rValue;
	POSITION posMsg = m_SendingMsgs.GetStartPosition();

	while (posMsg)
	{
		m_SendingMsgs.GetNextAssoc(posMsg, rKey, rValue);
		bool bHasSource = false;
		bool bHasAnonyStart = false;
		CMsgHopInfo info;
		for (int i = 0; i < MSG_HOP_STATE_MAX; ++i)
		{
			StateStatistics[i] += rValue->m_CnP[i].nCount;
			if (rValue->m_CnP[i].nCount == 0)
			{
				continue;
			}
			switch (i)
			{
			case MSG_HOP_STATE_SOURCE:
			{
				ASSERT(rValue->m_CnP[i].nCount == 1);
				break;
			}
			case MSG_HOP_STATE_ANONYMITY_END:
			{
				ASSERT(rValue->m_CnP[i].nCount == 1);
				ASSERT(rValue->m_CnP[MSG_HOP_STATE_SOURCE].nCount == 1);

				const CMsgInsideInfo & cntInfo = rValue->GetHopInfo();
				CDoublePoint dpAnonymityBegin = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_BEGIN].pos).m_Location;
				CDoublePoint dpAnonymityEnd = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].pos).m_Location;
				double fCurrentAnonyDistance = CDoublePoint::GetDistance(
						dpAnonymityBegin,
						dpAnonymityEnd
				);
				fTotalAnonyDistance += fCurrentAnonyDistance;
				if (fCurrentAnonyDistance > fMaxAnonyDistance)
				{
					fMaxAnonyDistance = fCurrentAnonyDistance;
				}
				fTotalObfuscationNum += rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_TRANS].nCount;
				ASSERT(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_TRANS].nCount > 0);

				SIM_TIME lnSourceTime = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_SOURCE].pos).m_Time;
				SIM_TIME lnAnonyEndTime = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].pos).m_Time;
				SIM_TIME lnAnonyTimeCost = lnAnonyEndTime - lnSourceTime;
				lnTotalAnonyTime += lnAnonyTimeCost;
				break;
			}
			case MSG_HOP_STATE_DESTINATION:
			{
				ASSERT(rValue->m_CnP[MSG_HOP_STATE_SOURCE].nCount == 1);

				const CMsgInsideInfo & cntInfo = rValue->GetHopInfo();

				SIM_TIME lnSourceTime = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_SOURCE].pos).m_Time;
				SIM_TIME lnDestinationTime = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_DESTINATION].pos).m_Time;
				SIM_TIME lnLatency = lnDestinationTime - lnSourceTime;
				lnTotalLatency += lnLatency;

				nInterHopCountTotle += cntInfo.GetHopCount();
				break;
			}
			}
		}
	}

	CStatisticsReport report;
	report.m_nStartedPackages = StateStatistics[MSG_HOP_STATE_SOURCE];
	report.m_nDeliveredPackages = StateStatistics[MSG_HOP_STATE_DESTINATION];
	report.m_nStartAnonyCount = StateStatistics[MSG_HOP_STATE_ANONYMITY_BEGIN];
	report.m_nFinishAnonyCount = StateStatistics[MSG_HOP_STATE_ANONYMITY_END];
	report.m_fAveTotalHops = nInterHopCountTotle * 1.0 / (report.m_nDeliveredPackages == 0 ? 1 : report.m_nDeliveredPackages);
	report.m_fAveLatency = lnTotalLatency * 0.001 / (report.m_nDeliveredPackages == 0 ? 1 : report.m_nDeliveredPackages);
	report.m_fAveAnonyDistance = fTotalAnonyDistance / (report.m_nFinishAnonyCount == 0 ? 1 : report.m_nFinishAnonyCount);
	report.m_fAveObfuscationNum = fTotalObfuscationNum / (report.m_nFinishAnonyCount == 0 ? 1 : report.m_nFinishAnonyCount);
	report.m_fMaxAnonyDistance = fMaxAnonyDistance;
	report.m_fAveAnonyTimeCost = lnTotalAnonyTime * 0.001 / (report.m_nFinishAnonyCount == 0 ? 1 : report.m_nFinishAnonyCount);

	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineMessageStatisticsChanged(report);
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

double CHostEngine::GetAveSurroundingHosts(double fRadius, int nComment)
{
	double fTotalHosts = 0;
	int nAnonyEndCount = 0;
	
	int rKey;
	CRoutingStatisticsInfo * rValue;
	POSITION posMsg = m_SendingMsgs.GetStartPosition();

	while (posMsg)
	{
		m_SendingMsgs.GetNextAssoc(posMsg, rKey, rValue);
		POSITION findResult = rValue->GetHopInfo().GetRecordWith(nComment);
		if (findResult)
		{
			++nAnonyEndCount;
			const CMsgHopInfo & findInfo = rValue->GetHopInfo().GetRecordAt(findResult);
			int nHostNumber = m_pRoadNet->GetHostNumberInRange(findInfo.m_Location, fRadius, findInfo.m_Time);
			fTotalHosts += nHostNumber;
		}
	}
	double fRet = fTotalHosts / nAnonyEndCount;
	return fRet;
}

int CHostEngine::GetSourceOnRing(double fInside, double fOutside)
{
	int nRet = 0;

	int rKey;
	CRoutingStatisticsInfo * rValue;
	POSITION posMsg = m_SendingMsgs.GetStartPosition();

	while (posMsg)
	{
		m_SendingMsgs.GetNextAssoc(posMsg, rKey, rValue);
		if (rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].nCount == 1)
		{
			ASSERT(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].nCount == 1);
			ASSERT(rValue->m_CnP[MSG_HOP_STATE_SOURCE].nCount == 1);
			const CMsgInsideInfo & cntInfo = rValue->GetHopInfo();
			CHost * pOriginalRequester = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_SOURCE].pos).m_pProtocol->GetHost();
			CDoublePoint dpAnonymityEnd = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].pos).m_Location;
			SIM_TIME lnEndTime = cntInfo.GetRecordAt(rValue->m_CnP[MSG_HOP_STATE_ANONYMITY_END].pos).m_Time;
			CDoublePoint dpReqerLocation = pOriginalRequester->GetPosition(lnEndTime);
			double fDistance = CDoublePoint::GetDistance(
				dpReqerLocation,
				dpAnonymityEnd
			);
			if (fDistance >= fInside && fDistance <= fOutside)
			{
				nRet++;
			}
		}
	}
	return nRet;
}

int CHostEngine::CheckEventList()
{
	if (!m_TransmitionMap.IsEmpty() || !m_TransmitionWaitingList.IsEmpty())
	{
		PostThreadMessage(MSG_ID_ENGINE_EVENT_CHANGED, 0, 0);
		return -1;
	}
// 	CString strLog;
// 	strLog.Format(_T("\nCheckEventList %d %d"), (int)m_lnSimTimeMillisecond, (int)m_lnEventCheckBoundary);
// 	OutputDebugString(strLog);
	int nProcCount = 0;
	while (m_lnSimTimeMillisecond < m_lnEventCheckBoundary)
	{
		POSITION pos = m_EventList.GetHeadPosition();
		if (!pos)
		{
			break;
		}
		CEngineEvent FirstEvent = m_EventList.GetHead();
		if (FirstEvent.m_lnSimTime < m_lnEventCheckBoundary)
		{
// 			CString strLog;
// 			strLog.Format(_T("\n-- %d %d"), FirstEvent.m_lnSimTime, FirstEvent.m_nCommandId);
// 			OutputDebugString(strLog);
			m_lnSimTimeMillisecond = FirstEvent.m_lnSimTime;
			FirstEvent.m_pUser->OnEngineTimer(FirstEvent.m_nCommandId);
			m_EventList.RemoveHead();
			++nProcCount;
		}
		else
		{
			break;
		}
	}
	return nProcCount;
}

bool CHostEngine::IsCheckEventSafe() const
{
	if (!m_TransmitionMap.IsEmpty() || !m_TransmitionWaitingList.IsEmpty())
	{
		OutputDebugString(_T("\n m_TransmitionMap or m_TransmitionWaitingList not empty"));
		return false;
	}
	int nProcCount = 0;
	while (m_lnSimTimeMillisecond < m_lnEventCheckBoundary)
	{
		POSITION pos = m_EventList.GetHeadPosition();
		if (!pos)
		{
			break;
		}
		CEngineEvent FirstEvent = m_EventList.GetHead();
		if (FirstEvent.m_lnSimTime < m_lnEventCheckBoundary)
		{
			CString strLog;
			strLog.Format(_T("\nCheck ERR %d %d"), (int)FirstEvent.m_lnSimTime, FirstEvent.m_nCommandId);
			OutputDebugString(strLog);
			return false;
		}
		else
		{
			break;
		}
	}
	return true;
}

void CHostEngine::OnCommonTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_PERIOD)
	{
		m_bWaitingTimer = false;
		if (m_bPaused == false)
		{
			TryNextPeriod(0, 0);
		}
	}
}

void CHostEngine::OnStartEngine(WPARAM wParam, LPARAM lParam)
{
	int nRunningTimer = sm_nWorkingTimer;
	WriteLog(_T("START"));
	if (m_bWaitingTimer == true)
	{
		CString strTimer;
		strTimer.Format(_T("Is waiting, remain %d    %d"), sm_Records.GetSize(), sm_LastError);
		WriteLog(strTimer);
	}
	m_bPaused = false;
	m_bWaitingTimer = true;
	UINT_PTR ret = SetCommonTimer(TIMER_ID_PERIOD, TIMER_LONG_PERIOD);
	if (ret == 0)
	{
		int nLastErr = GetLastError();
		CString strErr;
		strErr.Format(_T("Timer ERR %d"), nLastErr);
		WriteLog(strErr);
	}
	else if(ret == -123)
	{
		WriteLog(_T("Timer ERR logic"));
	}
}

void CHostEngine::OnResetEngine(WPARAM wParam, LPARAM lParam)
{
	WriteLog(_T("RESET"));
	m_bPaused = true;
	m_lnSimTimeMillisecond = 0;
}

void CHostEngine::OnPauseEngine(WPARAM wParam, LPARAM lParam)
{
	WriteLog(_T("PAUSE"));
	m_bPaused = true;
}

void CHostEngine::OnFinishedOneForecast(WPARAM wParam, LPARAM lParam)
{
	SIM_TIME * pMsg = (SIM_TIME *)wParam;
	SIM_TIME lnDiffer = GetForecastThreshhold();
	if (GetSimTime() < *pMsg || GetSimTime() - *pMsg > lnDiffer)
	{
		delete pMsg;
		return;
	}
	CMsgPosFrcstReport * pReport = m_pForecastThread->GetReport(GetSimTime(), lnDiffer);
	while (!m_TransmitionWaitingList.IsEmpty())
	{
		CTransmitionRecord tmpRecord = m_TransmitionWaitingList.GetHead();
		StartJudgeProcess(tmpRecord, 2);
		m_TransmitionWaitingList.RemoveHead();
	}
	delete pMsg;
	pMsg = NULL;
}

void CHostEngine::OnFinishedForecastOnce(SIM_TIME lnSimTime)
{
	SIM_TIME * pMsg = new SIM_TIME(lnSimTime);
	PostThreadMessage(MSG_ID_ENGINE_FORECAST_OK, (WPARAM)pMsg, 0);
}

void CHostEngine::SendForecastCommand(SIM_TIME lnSimTime)
{
	CMsgPosFrcstNewTime * pForecastMsg = new CMsgPosFrcstNewTime();
	pForecastMsg->m_lnSimTime = lnSimTime;
	m_pForecastThread->PostThreadMessage(MSG_ID_POS_FRCST_NEW_TIME, (WPARAM)pForecastMsg, 0);
	m_lnLastForecastSimTime = pForecastMsg->m_lnSimTime;
}

void CHostEngine::ForecastSeveralPeriod(int nPeriodCount)
{
	double fDiffer = GetForecastThreshhold();
	for (int i = 0; i < nPeriodCount; ++i)
	{
		double fFutureSecond = i * GetPeriodDefaultInterval();
		double fForecastSecond = m_lnSimTimeMillisecond + fFutureSecond;
		if (fForecastSecond - m_lnLastForecastSimTime > fDiffer)
		{
			SendForecastCommand(fForecastSecond);
		}
	}
}

void CHostEngine::DeleteForecastsBefore(int nSecondLong)
{
	CMsgPosFrcstComplete * pMsg = new CMsgPosFrcstComplete();
	pMsg->m_lnSimTime = m_lnSimTimeMillisecond - nSecondLong;
	m_pForecastThread->PostThreadMessage(MSG_ID_POS_FRCST_COMPLETE, (WPARAM)pMsg, 0);
}

SIM_TIME CHostEngine::GetForecastThreshhold()
{
	return m_pRoadNet->GetSimTimeCrossHalfBlank();
}

SIM_TIME CHostEngine::GetPeriodDefaultInterval()
{
	return TIMER_LONG_PERIOD * m_lnSimMillisecondPerActualSecond / 1000.0;
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
}

void CHostEngine::OnEventListChanged(WPARAM wParam, LPARAM lParam)
{
	CheckEventList();
}

void CHostEngine::TryNextPeriod(WPARAM wParam, LPARAM lParam)
{
	if (m_bPaused == false)
	{
		bool bCheck = IsCheckEventSafe();
		if (bCheck)
		{
			OnEveryPeriod();
			m_bWaitingTimer = true;
			UINT_PTR ret = SetCommonTimer(TIMER_ID_PERIOD, TIMER_LONG_PERIOD);
			if (ret == 0)
			{
				int nLastErr = GetLastError();
				CString strErr;
				strErr.Format(_T("Timer ERR %d"), nLastErr);
				WriteLog(strErr);
			}
			else if (ret == -123)
			{
				WriteLog(_T("Timer ERR logic"));
			}
		}
		else
		{
			PostThreadMessage(MSG_ID_ENGINE_TRY_NEXT_PERIOD, 0, 0);
		}
	}
}

void CHostEngine::IncreaseSpeed(WPARAM wParam, LPARAM lParam)
{
	m_lnSimMillisecondPerActualSecond *= 1.5;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineSpeedChanged();
	}
}

void CHostEngine::DecreaseSpeed(WPARAM wParam, LPARAM lParam)
{
	m_lnSimMillisecondPerActualSecond /= 1.5;
	POSITION pos = m_NotifyList.GetHeadPosition();
	while (pos)
	{
		m_NotifyList.GetNext(pos)->OnEngineSpeedChanged();
	}
}

BEGIN_MESSAGE_MAP(CHostEngine, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_START, OnStartEngine)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_RESET, OnResetEngine)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_PAUSE, OnPauseEngine)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_FORECAST_OK, OnFinishedOneForecast)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_JUDGE_OK, OnJudgeOk)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_EVENT_CHANGED, OnEventListChanged)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_TRY_NEXT_PERIOD, TryNextPeriod)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_SPEED_UP, IncreaseSpeed)
	ON_THREAD_MESSAGE(MSG_ID_ENGINE_SPEED_DOWN, DecreaseSpeed)
END_MESSAGE_MAP()


// CHostEngine 消息处理程序
