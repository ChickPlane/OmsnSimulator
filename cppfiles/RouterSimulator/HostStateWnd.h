#pragma once
#include "DlgHostProperty.h"


// CHostStateWnd

class CHostStateWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CHostStateWnd)

public:
	CHostStateWnd();
	virtual ~CHostStateWnd();

	void AdjustLayout();
	CDlgHostProperty * m_pDlgProperty;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


