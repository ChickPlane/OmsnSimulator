#pragma once
#include "afxwin.h"
#include "Host.h"
#include "HostInfo.h"
#include "RoadNet.h"


// CDlgHostInfo �Ի���

class CDlgHostInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHostInfo)

public:
	CDlgHostInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgHostInfo();

	void SetData(CHost * pHost, CRoadNet * pRoadNet);

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlEditHostId;
	afx_msg void OnBnClickedButton1();

private:
	CHost * m_pHost;
	CRoadNet * m_pRoadNet;
	CHostInfo m_Info;
public:
	virtual BOOL OnInitDialog();
	CEdit m_ctrlEditTargetNum;
	CListBox m_ctrlMsgs;
	bool m_bFollow;

	void UpdateUi();
	afx_msg void OnBnClickedButtonRefresh();
	CButton m_ctrlCheckFollow;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckFollow();
};
