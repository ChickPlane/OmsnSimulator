#pragma once
#include "CommonTimer.h"
#include "PositionForecastUser.h"
#include "EngineUser.h"
#include "EngineEvent.h"
#include "TransmitionRecord.h"
#include "HostGui.h"
#include "TestTimer.h"
#include "StatisticSummary.h"

class CRoadNet;
class CMapGui;
class CPositionForecast;
class CConnectionJudge;
class CMsgCntJudgeReceiverReport;
class CMsgNewSendJudge;

enum {
	MSG_ID_ENGINE_START = WM_USER + 3050,
	MSG_ID_ENGINE_PAUSE,
	MSG_ID_ENGINE_RESUME,
	MSG_ID_ENGINE_RESET,
	MSG_ID_ENGINE_FORECAST_OK,
	MSG_ID_ENGINE_JUDGE_OK,
	MSG_ID_ENGINE_EVENT_CHANGED,
	MSG_ID_ENGINE_SPEED_UP,
	MSG_ID_ENGINE_SPEED_DOWN,
	MSG_ID_ENGINE_WATCH_TIME
};

enum {
	TIMER_ID_PERIOD = 1000
};

#define TIMER_LONG_PERIOD 50

// CHostEngine

class CHostEngine : public CWinThread, public CCommonTimer, public CPositionForecastUser
{
	DECLARE_DYNCREATE(CHostEngine)

protected:
	CHostEngine();           // 动态创建所使用的受保护的构造函数
	virtual ~CHostEngine();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void SetValue(CRoadNet * pRoadNet, CMapGui * pMapGui);
	void BreakMapGui();
	// User malloc pMsg, Engine free it.
	void TransmitMessage(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CYell * pMsg);
	void RegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds);
	SIM_TIME GetSimTime() const;
	void WriteLog(const CString & strLog);
	int GetSpeed() const;
	int GetActualSpeed() const;
	CRoadNet * GetRoadNet();
	void RegisterUser(CEngineUser * pUser);
	void ChangeSummary(const CStatisticSummary & summary);
	CStatisticSummary & GetSummary();

	//double GetAveSurroundingHosts(double fRadius, int nComment);
	//int GetSourceOnRing(double fInside, double fOutside);
	CHost * GetHost(int nHostId) const;
	void SetCommunicationRadius(double fRadius) { m_fCommunicationRadius = fRadius; }

protected:
	void RegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds, BOOL bStack);
	void OnEveryPeriod();
	int CheckEventList();
	void OnCommonTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
	void OnStartEngine(WPARAM wParam, LPARAM lParam);
	void OnResetEngine(WPARAM wParam, LPARAM lParam);
	void OnPauseEngine(WPARAM wParam, LPARAM lParam);
	void OnResumeEngine(WPARAM wParam, LPARAM lParam);
	void OnFinishedOneForecast(WPARAM wParam, LPARAM lParam);
	void OnJudgeOk(WPARAM wParam, LPARAM lParam);
	void OnEventListChanged(WPARAM wParam, LPARAM lParam);

	void IncreaseSpeed(WPARAM wParam, LPARAM lParam);
	void DecreaseSpeed(WPARAM wParam, LPARAM lParam);

	void WatchTime(WPARAM wParam, LPARAM lParam);

	void OnFirstForecastFinished();
	void OnFinishedForecastOnce(SIM_TIME lnSimTime);
	BOOL SendForecastCommand(SIM_TIME lnSimTime);
	void DeleteForecastsBefore(int nBeforeSecond);
	void StartJudgeProcess(const CTransmitionRecord & transmitionData, int nFrom);
	void StartJudgeUnicastProcess(const CTransmitionRecord & transmitionData);

	SIM_TIME GetForecastThreshhold(int nExtraBlock) const;
	SIM_TIME GetPeriodDefaultInterval();
	void RefreshUi();
	void RefreshUiDirectly(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb);
	int GetSurringNodesCount(CDoublePoint currentLocation, double fRadius);

	CMsgCntJudgeReceiverReport * GetUnicastReport(const CTransmitionRecord & tr);
	SIM_TIME GetActualSimMillisecPerActSec();
	SIM_TIME GetBoundary() const;
	void NotifyTimeChange();
	void UpdateStartTick();
	void JudgeAllUnicastOk();
	void JudgeAllFullForcast();
	void JudgeAllBroadcast(const CMsgCntJudgeReceiverReport * pMsg);
	BOOL PreJudgeAllHosts(SIM_TIME lnTime);
	BOOL DeletePreFullJudge(SIM_TIME lnTime);
	void SendEventChangeMsg();
	void SendJudgeOkMsg();
	void SendJudgeMsgToThread(CMsgNewSendJudge * pJudgeMsg);

private:
	SIM_TIME m_lnSimTimeMillisecond;
	SIM_TIME m_lnEventCheckBoundary;
	SIM_TIME m_lnEventCheckBoundaryExpect;
	SIM_TIME m_lnExpectSimMillisecPerActSec;
	CRoadNet * m_pRoadNet;
	CMapGui * m_pMapGui;
	CPositionForecast * m_pForecastThread;
	CConnectionJudge * m_pJudge;
	CArray<CConnectionJudge *> m_JudgeList;
	int m_nJudgeMax;
	BOOL m_bEnableMonitor;

	ULONGLONG m_ulStartTickCount;
	SIM_TIME m_lnSimTimeTickCountStart;

	bool m_bPaused;
	BOOL m_blimitedSpeed;
	BOOL m_bWaitingActualTime;
	BOOL m_bFinishFirstForecast;

	SIM_TIME m_lnLastForecastSimTime;
	SIM_TIME m_lnLastForecastedSimTime;

	CList<CEngineEvent> m_EventList;
	//CMap<int, int, CTransmitionRecord, CTransmitionRecord&> m_TransmitionMap;
	CList<CTransmitionRecord> m_TransmitionUnicast;
	CList<CTransmitionRecord> m_TransmitionBroadcast;
	int m_nMsgId;
	ULONGLONG m_llLastUpdateTime;
	CList<CEngineUser *> m_NotifyList;
	int m_nTransmitCount;

	double m_fCommunicationRadius;

	CTestTimer m_tt;
	CStatisticSummary m_Summary;
	SIM_TIME m_ulLastNotifyTime;
	BOOL m_bCheckingEvents;
	CMsgNewSendJudge * m_pJudgeMsg;
	int m_nJudgeRunTime;
	BOOL m_bJudgingUnicast;
	CMap<SIM_TIME, SIM_TIME, const CMsgCntJudgeReceiverReport*, const CMsgCntJudgeReceiverReport*> m_FullJudgeRecord;

	int m_nMsgCount;
	int m_nJudgeOkMsgCount;
	int m_nBusyJudgeThreadCount;
private:
	ULONGLONG m_aaa;

};


