#pragma once
#include "CommonTimer.h"
#include "PositionForecastUser.h"
#include "EngineUser.h"
#include "EngineEvent.h"
#include "TransmitionRecord.h"
#include "HostGui.h"
#include "RoutingStatisticsInfo.h"
#include "TestTimer.h"

class CRoadNet;
class CMapGui;
class CPositionForecast;
class CConnectionJudge;
class CMsgCntJudgeReceiverReport;
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
	void RecordPackageStateChange(int nDataId, const CMsgInsideInfo & msgInfo, int nState);
	CRoadNet * GetRoadNet();
	void RegisterUser(CEngineUser * pUser);

	double GetAveSurroundingHosts(double fRadius, int nComment);
	int GetSourceOnRing(double fInside, double fOutside);
	CHost * GetHost(int nHostId) const;
	void SetCommunicationRadius(double fRadius) { m_fCommunicationRadius = fRadius; }

	static VOID CALLBACK timerFunEE(HWND wnd, UINT msg, UINT_PTR id, DWORD d);
protected:
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

	SIM_TIME GetForecastThreshhold(int nExtraBlock) const;
	SIM_TIME GetPeriodDefaultInterval();
	void RefreshUi();
	void RefreshUiDirectly(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb);
	bool RefreshUiOptimize(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb);
	void ReportMessageStatisticsChanged();
	int GetSurringNodesCount(CDoublePoint currentLocation, double fRadius);

	CMsgCntJudgeReceiverReport * GetUnicastReport(const CTransmitionRecord & tr);
	SIM_TIME GetActualSimMillisecPerActSec();
	SIM_TIME GetBoundary() const;
	void NotifyTimeChange();

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
	CList<CTransmitionRecord> m_TransmitionWaitingList;
	CMap<int, int, CTransmitionRecord, CTransmitionRecord&> m_TransmitionMap;
	int m_nMsgId;
	ULONGLONG m_llLastUpdateTime;
	CList<CEngineUser *> m_NotifyList;
	int m_nTransmitCount;

	CMap<int, int, CRoutingStatisticsInfo *, CRoutingStatisticsInfo *> m_SendingMsgs;
	double m_fCommunicationRadius;

	CTestTimer m_tt;
};


