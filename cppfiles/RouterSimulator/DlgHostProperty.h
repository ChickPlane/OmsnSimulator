#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CDlgHostPptMsg;
class CHost;
class CHostEngine;

class CHostPropertyParam
{
public:
	CHost * m_pHost;
	CHostEngine * m_pEngine;
};

enum
{
	HOST_PPT_MSG_SHOWHOST = WM_USER + 2000,
};

// CDlgHostProperty 对话框

class CDlgHostProperty : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHostProperty)

public:
	CDlgHostProperty(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHostProperty();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HOST_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTabHstPptDetails(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl m_ctrlTab;
	CDlgHostPptMsg * m_pDlgMsg;
	CHostPropertyParam m_RunParam;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnHostPptMsgShowhost(WPARAM wParam, LPARAM lParam);
public:
	CEdit m_ctrlEditSelfId;
	CEdit m_ctrlEditReceiveId;
	afx_msg void OnBnClickedButtonHstPptSend();
	CEdit m_ctrlEditTimeOut;
};
