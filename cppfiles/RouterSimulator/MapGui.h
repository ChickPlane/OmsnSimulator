#pragma once
#include "RoadNet.h"
#include "RoadPointStateInfo.h"
#include "PictureBackup.h"
#include "HostGui.h"
#include "SimulatorCfg.h"

class CHostEngine;


enum {
	MSG_ID_MAP_UPDATE_HOSTS = WM_USER + 200,
	MSG_ID_INIT_OK,
	MSG_ID_WRITE_LOG,
};

#define MAP_GUI_TIMER_ID_CHECK_TIMEOUT 1253
#define MAP_GUI_TIMER_ID_CHECK_WORKING 1254
#define MAP_GUI_TIMER_ID_START_TEST    1255


// CMapGui 视图

class CMapGui : public CScrollView
{
	DECLARE_DYNCREATE(CMapGui)

public:
	void SetScale(int nScaleIndex);
	void SetRoadNet(CRoadNet * pRoadNet);
	void SetEngine(CHostEngine * pEngine);
	void RefreshUi(bool bRedrawAll);
	void GetRefreshArea(CDoublePoint & lt, CDoublePoint & rb);
	CSimulatorCfg m_Cfg;
	CScrollView * m_pView;

protected:
	CMapGui();           // 动态创建所使用的受保护的构造函数
	virtual ~CMapGui();


public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	const double sm_fRoadHalfWidth = 3.25;//7.5;
	const double sm_fRoadWidth = sm_fRoadHalfWidth * 2;
	const double sm_fIntersectionRadius = 15;
	const double sm_fConnectWidth = sm_fRoadHalfWidth/2;
	const double sm_fRoadMarkLineWidth = 0.5;
	const double sm_fRoadMarkRadius = sm_fIntersectionRadius / 2;
	const double sm_fMargin = 100.0;

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	void GetResonableRange(int & nWidth, int & nHeight);
	void GetBackupPictureRange(int & nWidth, int & nHeight, int nScaleIndex);
	void DrawRoads(CDC * pMemDC);
	void DrawIntersections(CDC & MemDC, CBitmap & MemBitmap);
	void DrawRoadConnections(CDC & MemDC, CBitmap & MemBitmap);
	void DrawAllDijNodes(CDC * pMemDC);
	void DrawSchedules(CDC * pMemDC);
	void DrawOneSchedule(CDC * pMemDC, int nHostId, CDoublePoint & lt, CDoublePoint & rb);
	void DrawHosts(CDC * pMemDC);
	void DrawHosts_AptCard(CDC * pMemDC);
	void DrawFollowings(CDC * pMemDC);

	void DrawRoadPoint(CDC * pMemDC);
	void DrawCircleAroundPoints(CDC * pMemDC);
	void DrawConnectionForMarked(CDC & MemDC, CBitmap & MemBitmap);
	void DrawShortPathDirected(CDC & MemDC, CBitmap & MemBitmap);
	void DrawShortPathPair(CDC & MemDC, CBitmap & MemBitmap);
	void DrawOneSingleShortPath(CDC & MemDC, CBitmap & MemBitmap, int nFromId, int nToId);
	void DrawDijConnections(CDC & MemDC, CBitmap & MemBitmap);
	void DrawAllBuildings(CDC & MemDC, CBitmap & MemBitmap);
	void DrawBuildingsToRoads(CDC & MemDC, CBitmap & MemBitmap);
	void DrawExtreInImage();


	void GeographicToGraphicPoint(const CDoublePoint & actualPoint, CPoint & showPoint);
	void GraphicToGeographicPoint(const CPoint & showPoint, CDoublePoint & actualPoint);
	int GetIntersectionRadius();
	CDoublePoint GetMousePosInMapScale(CPoint pt);
	CDoublePoint ScreenToMapPoint(CPoint pt);
	CPoint GetNewScrollPos();
	void CreateRandomPackages(int nNumber, SIM_TIME lnTimeOut);

	void BitBlt(CDC * pDC);
	CPictureBackup * GetBackup(int nScaleIndex, int nBackupVersion);
	CRect GetRefreshArea();
	void InitBackup();
	void DestroyBackup();
	double GetScale();
	CHost * GetNearestHost(const CDoublePoint & hostPos);

	// m/pixel
	double m_fScale;
	CArray<CPictureBackup *> m_Backup;
	CArray<double> m_fScales;
	int m_nScaleIndex;
	int m_nBackupVersion;
	int m_nIntersectionRadius;
	CDoublePoint m_LeftTop;
	CDoublePoint m_RightBottom;
	CRoadNet * m_pRoadNet;
	CSize m_sizeTotal;
	CDoublePoint m_MousePosScale;
	CPoint m_LastMousePosition;

	CDC m_MemDC;

	CArray<CRoadPointStateInfo> m_RoadPointInfos;
	CList<PtrRoadPoint> m_CircleRoadPoints;
	int m_nDijFinishedCount;


	int m_nLastSelectedId;
	CArray<CHostGui> * m_pHostNeedUpdate;
	CHostEngine * m_pEngine;

	COLORREF m_BackgroundColor;
	bool m_bEngineShouldBeRunning;
	bool m_bEnginePaused;
	ULONGLONG m_llLastDrawTime;

	CHost * m_pFollowHost;

	SIM_TIME m_lnLastEngineTime;
	int m_nRestartTimes;
	SIM_TIME m_lnExpectEndTime;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
protected:
	afx_msg LRESULT OnMarkfinisheddijnodes(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateHosts(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAllInitOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWriteLog(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBuildingOpen();
	afx_msg void OnEngineRun();
	afx_msg void OnEngineReset();
	afx_msg void OnEnginePause();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


