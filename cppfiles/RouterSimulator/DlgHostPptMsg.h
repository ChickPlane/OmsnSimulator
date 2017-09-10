#pragma once

class CListCtrlMessages;
class CHostPropertyParam;
class CListCtrlProcessInfo;

#define DLG_HOST_PPT_TIMER_ID 17

// CDlgHostPptMsg 对话框

class CDlgHostPptMsg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHostPptMsg)

public:
	CDlgHostPptMsg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHostPptMsg();

	void ResetData(CHostPropertyParam * pParam);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HOST_INFO_MSG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CListCtrlMessages * m_pGrid;
	CListCtrlProcessInfo * m_pProcessInfo;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
