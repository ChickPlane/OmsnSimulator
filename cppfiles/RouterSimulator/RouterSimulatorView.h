
// RouterSimulatorView.h : CRouterSimulatorView 类的接口
//

#pragma once
#include "MapGui.h"
#include "HostEngine.h"
#include "EngineUser.h"
#include "StatisticSummary.h"

#define DYN_IDC_CPY_SUMMARY 22335
#define DYN_IDC_CREATE_MSGS 22334

class CRouterSimulatorView : public CScrollView, public CEngineUser
{
protected: // 仅从序列化创建
	CRouterSimulatorView();
	DECLARE_DYNCREATE(CRouterSimulatorView)

// 特性
public:
	CRouterSimulatorDoc* GetDocument() const;
	CMapGui * m_pMapGui;
	CHostEngine * m_pEngine;
	CButton * m_pBtnCopySummary;
	CEdit * m_pEditSpeed;
	CEdit * m_pEditMsgStatistic;
	CEdit * m_pEditPickCount;
	CEdit * m_pEditTimeOut;
	CEdit * m_pEditLabel;
	CButton * m_pBtnCreateMsgs;
	CEdit * m_pAveLatency;
	CEdit * m_pAveAnonyDistance;
	CEdit * m_pAveAnonyTime;
	CStatisticSummary m_Summare;

// 操作
public:

// 重写
public:
	virtual void OnEngineTimer(int nCommandId);
	virtual void OnEngineSpeedChanged();
	virtual void OnEngineMessageStatisticsChanged(const CStatisticsReport & report);
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	void DestroyEngine();

	void InitHostProtocol(int nCopyCount, char * strProtocolName, double fCommunicateRadius, int nK, double fHigh, double fLow, double fAnonymityRadius);

// 实现
public:
	virtual ~CRouterSimulatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnButtonCreateMsgs();
	afx_msg void OnButtonCpySummary();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnAfteronedijnode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDataPrepareFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTestComplete(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // RouterSimulatorView.cpp 中的调试版本
inline CRouterSimulatorDoc* CRouterSimulatorView::GetDocument() const
   { return reinterpret_cast<CRouterSimulatorDoc*>(m_pDocument); }
#endif

