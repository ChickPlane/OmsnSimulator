#pragma once
#include "CommonTimer.h"
#include "PositionForecastUser.h"
#include "RoutingMsg.h"
#include "EngineUser.h"
#include "EngineEvent.h"
#include "TransmitionRecord.h"
#include "HostGui.h"
#include "RoutingStatisticsInfo.h"

class CRoadNet;
class CMapGui;
class CPositionForecast;
class CConnectionJudge;

enum {
	MSG_ID_ENGINE_START = WM_USER + 3050,
	MSG_ID_ENGINE_PAUSE,
	MSG_ID_ENGINE_RESET,
	MSG_ID_ENGINE_FORECAST_OK,
	MSG_ID_ENGINE_JUDGE_OK,
	MSG_ID_ENGINE_EVENT_CHANGED,
	MSG_ID_ENGINE_TRY_NEXT_PERIOD,
	MSG_ID_ENGINE_SPEED_UP,
	MSG_ID_ENGINE_SPEED_DOWN,
};

enum {
	TIMER_ID_PERIOD = 1000
};

#define TIMER_LONG_PERIOD 60

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
	void TransmitMessage(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CRoutingMsg * pMsg);
	void RegisterTimer(int nCommandId, CEngineUser * pUser, SIM_TIME lnLaterMilliseconds);
	SIM_TIME GetSimTime() const;
	void WriteLog(const CString & strLog);
	int GetSpeed();
	void RecordPackageStateChange(int nDataId, const CMsgInsideInfo & msgInfo, int nState);
	CRoadNet * GetRoadNet();
	void RegisterUser(CEngineUser * pUser);

	double GetAveSurroundingHosts(double fRadius, int nComment);
	int GetSourceOnRing(double fInside, double fOutside);

protected:
	void OnEveryPeriod();
	int CheckEventList();
	bool IsCheckEventSafe()const;
	void OnCommonTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	void OnStartEngine(WPARAM wParam, LPARAM lParam);
	void OnResetEngine(WPARAM wParam, LPARAM lParam);
	void OnPauseEngine(WPARAM wParam, LPARAM lParam);
	void OnFinishedOneForecast(WPARAM wParam, LPARAM lParam);
	void OnJudgeOk(WPARAM wParam, LPARAM lParam);
	void OnEventListChanged(WPARAM wParam, LPARAM lParam);
	void TryNextPeriod(WPARAM wParam, LPARAM lParam);

	void IncreaseSpeed(WPARAM wParam, LPARAM lParam);
	void DecreaseSpeed(WPARAM wParam, LPARAM lParam);
	
	void OnFinishedForecastOnce(SIM_TIME lnSimTime);
	void SendForecastCommand(SIM_TIME lnSimTime);
	void ForecastSeveralPeriod(int nPeriodCount);
	void DeleteForecastsBefore(int nSecondLong);
	void StartJudgeProcess(const CTransmitionRecord & transmitionData, int nFrom);

	SIM_TIME GetForecastThreshhold();
	SIM_TIME GetPeriodDefaultInterval();
	void RefreshUi();
	void RefreshUiDirectly(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb);
	bool RefreshUiOptimize(CArray<CHostGui> * pMessage, const CDoublePoint & lt, const CDoublePoint & rb);
	void ReportMessageStatisticsChanged();
	int GetSurringNodesCount(CDoublePoint currentLocation, double fRadius);

private:
	SIM_TIME m_lnSimTimeMillisecond;
	SIM_TIME m_lnEventCheckBoundary;
	SIM_TIME m_lnSimMillisecondPerActualSecond;
	CRoadNet * m_pRoadNet;
	CMapGui * m_pMapGui;
	CPositionForecast * m_pForecastThread;
	CConnectionJudge * m_pJudge;
	CArray<CConnectionJudge *> m_JudgeList;
	int m_nJudgeMax;

	bool m_bPaused;
	bool m_bWaitingTimer;

	SIM_TIME m_lnLastForecastSimTime;
	SIM_TIME m_lnLastForecastedSimTime;

	CList<CEngineEvent> m_EventList;
	CList<CTransmitionRecord> m_TransmitionWaitingList;
	CMap<int, int, CTransmitionRecord, CTransmitionRecord&> m_TransmitionMap;
	int m_nMsgId;
	ULONGLONG m_llLastUpdateTime;
	CList<CEngineUser *> m_NotifyList;

	CMap<int, int, CRoutingStatisticsInfo *, CRoutingStatisticsInfo *> m_SendingMsgs;
};


