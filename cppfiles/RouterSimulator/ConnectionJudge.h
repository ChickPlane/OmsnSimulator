#pragma once
#include "PositionForecastUser.h"
#include "MsgCntJudgeReceiverReport.h"

class CRoadNet;
class CHost;
class CPositionForecast;
class CHostEngine;
class CMsgPosFrcstReport;

class CMsgNewJudgeItem
{
public:
	CHost * m_pHost;
	double m_fSecondId;
	double m_fRadius;
};

class CMsgNewSendJudge
{
public:
	CMsgNewSendJudge() :m_bFullJudge(FALSE) {}
	double GetSecondId();
	BOOL m_bFullJudge;
	CList<CMsgNewJudgeItem> m_Items;
// 	CMsgNewSendJudge() :m_pHost(NULL), m_fSecondId(0), m_fRadius(0), m_nMsgId(0) {}
// 	CHost * m_pHost;
// 	double m_fSecondId;
// 	double m_fRadius;
// 	int m_nMsgId;
};

enum {
	MSG_ID_JUDGE_FORECAST_OK = WM_USER + 100,
	MSG_ID_JUDGE_NEW_SEND
};

// CConnectionJudge

class CConnectionJudge : public CWinThread, public CPositionForecastUser
{
	DECLARE_DYNCREATE(CConnectionJudge)

protected:
	CConnectionJudge();           // 动态创建所使用的受保护的构造函数
	virtual ~CConnectionJudge();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void SetData(CRoadNet * pData);
	void SetForecast(CPositionForecast * pForecast);
	void SetEngine(CHostEngine * pEngine);
	void OnFinishedForecastOnce(SIM_TIME lnSimTime);

protected:
	DECLARE_MESSAGE_MAP()
	void OnFinishedOneForecast(WPARAM wParam, LPARAM lParam);
	void OnNewSend(WPARAM wParam, LPARAM lParam);

protected:
	void JudgeItem(const CMsgNewJudgeItem & item, CMsgPosFrcstReport * pReport, CReceiverReportItem & ret);
protected:
	CRoadNet * m_pData;
	CPositionForecast * m_pForecast;
	CHostEngine * m_pEngine;
};


