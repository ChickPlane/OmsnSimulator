#pragma once
#include "DoublePoint.h"
#include "HostReference.h"
#include "SimulatorCommon.h"

class CPositionForecastUser;
class CRoadNet;

enum {
	MSG_ID_POS_FRCST_NEW_TIME = WM_USER + 100,
	MSG_ID_POS_FRCST_COMPLETE,
	MSG_ID_POS_FRCST_REMOVEALL
};

class CMsgPosFrcstNewTime
{
public:
	CMsgPosFrcstNewTime();
	CMsgPosFrcstNewTime(const CMsgPosFrcstNewTime & src);
	CMsgPosFrcstNewTime & operator = (const CMsgPosFrcstNewTime & src);
	SIM_TIME m_lnSimTime;
};

class CMsgPosFrcstComplete
{
public:
	CMsgPosFrcstComplete();
	CMsgPosFrcstComplete(const CMsgPosFrcstComplete & src);
	CMsgPosFrcstComplete & operator = (const CMsgPosFrcstComplete & src);
	SIM_TIME m_lnSimTime;
};

class CMsgPosFrcstReport
{
public:
	CMsgPosFrcstReport();
	CMsgPosFrcstReport(const CMsgPosFrcstReport & src);
	CMsgPosFrcstReport & operator = (const CMsgPosFrcstReport & src);
	SIM_TIME m_lnSimTime;
	CMap<int, int, CHostReference, CHostReference &> m_Reference;
};

// CPositionForecast

class CPositionForecast : public CWinThread
{
	DECLARE_DYNCREATE(CPositionForecast)

protected:
	CPositionForecast();           // 动态创建所使用的受保护的构造函数
	virtual ~CPositionForecast();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetData(CRoadNet * pNet);
	void LockReports();
	void UnlockReports();
	void AddUser(CPositionForecastUser * pUser);
	void DelUser(CPositionForecastUser * pUser);

	CMsgPosFrcstReport * GetReport(SIM_TIME lnSimTime, SIM_TIME lnDiffer);

protected:
	DECLARE_MESSAGE_MAP()
	void OnForecastNewTime(WPARAM wParam, LPARAM lParam);
	void OnForecastComplete(WPARAM wParam, LPARAM lParam);
	void OnForecastRemoveAll(WPARAM wParam, LPARAM lParam);

protected:
	void DeleteRecords(SIM_TIME lnSimTimeBefore);
	void DoNewForecast(SIM_TIME lnSimTime);
protected:
	CList<CMsgPosFrcstReport * > m_Reports;
	CRoadNet * m_pData;
	CMutex m_MutexReport;
	CMutex m_MutexUser;
	CList<CPositionForecastUser *> m_Users;
};


