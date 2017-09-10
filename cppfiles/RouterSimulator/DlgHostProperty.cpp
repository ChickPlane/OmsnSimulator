// DlgHostProperty.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "DlgHostProperty.h"
#include "afxdialogex.h"
#include "DlgHostPptMsg.h"
#include "Host.h"
#include "HostEngine.h"
#include "RoadNet.h"


// CDlgHostProperty 对话框

IMPLEMENT_DYNAMIC(CDlgHostProperty, CDialogEx)

CDlgHostProperty::CDlgHostProperty(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_HOST_INFO, pParent)
	, m_pDlgMsg(NULL)
{

}

CDlgHostProperty::~CDlgHostProperty()
{
}

void CDlgHostProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_HST_PPT_DETAILS, m_ctrlTab);
	DDX_Control(pDX, IDC_EDIT_HST_PPT_SELFID, m_ctrlEditSelfId);
	DDX_Control(pDX, IDC_EDIT_HST_PPT_RECEIVER_ID, m_ctrlEditReceiveId);
	DDX_Control(pDX, IDC_EDIT_DLG_HST_PPT_TIME_OUT, m_ctrlEditTimeOut);
}


BEGIN_MESSAGE_MAP(CDlgHostProperty, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_HST_PPT_DETAILS, &CDlgHostProperty::OnTcnSelchangeTabHstPptDetails)
	ON_WM_CREATE()
	ON_MESSAGE(HOST_PPT_MSG_SHOWHOST, &CDlgHostProperty::OnHostPptMsgShowhost)
	ON_BN_CLICKED(IDC_BUTTON_HST_PPT_SEND, &CDlgHostProperty::OnBnClickedButtonHstPptSend)
END_MESSAGE_MAP()


// CDlgHostProperty 消息处理程序


void CDlgHostProperty::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	// TODO: 在此处添加消息处理程序代码
	if (IsWindow(m_ctrlTab.GetSafeHwnd()))
	{
		CRect rctTab;
		m_ctrlTab.GetWindowRect(&rctTab);
		CRect rctClient;
		GetClientRect(&rctClient);
		ScreenToClient(&rctTab);
		rctTab.bottom = rctClient.bottom - rctTab.left;
		rctTab.right = rctClient.right - 1*rctTab.left;
		m_ctrlTab.MoveWindow(rctTab);

		m_ctrlTab.GetClientRect(rctTab);
		//ScreenToClient(&rctTab);
		rctTab.DeflateRect(10, 35, 10, 10);
		m_pDlgMsg->MoveWindow(rctTab);
	}
}


void CDlgHostProperty::OnTcnSelchangeTabHstPptDetails(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


int CDlgHostProperty::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


BOOL CDlgHostProperty::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlTab.InsertItem(0, _T("Messages"));
	m_pDlgMsg = new CDlgHostPptMsg();
	m_pDlgMsg->Create(IDD_DIALOG_HOST_INFO_MSG, &m_ctrlTab);
	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_ctrlEditTimeOut.SetWindowText(_T("1000"));
	m_ctrlEditReceiveId.SetWindowText(_T("0"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT CDlgHostProperty::OnHostPptMsgShowhost(WPARAM wParam, LPARAM lParam)
{
	CHostPropertyParam * pParam = (CHostPropertyParam *)wParam;
	m_pDlgMsg->ResetData(pParam);
	m_RunParam = *pParam;
	delete pParam;
	CString strSelfId;
	strSelfId.Format(_T("%d"), m_RunParam.m_pHost->m_nId);
	m_ctrlEditSelfId.SetWindowText(strSelfId);
	return 0;
}


void CDlgHostProperty::OnBnClickedButtonHstPptSend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strRcv;
	m_ctrlEditReceiveId.GetWindowText(strRcv);
	int nId = _ttoi(strRcv);
	CString strTimeOut;
	m_ctrlEditTimeOut.GetWindowText(strTimeOut);
	int nTimeOut = _ttoi(strTimeOut);
	
	CHost * pHostFrom = m_RunParam.m_pHost;
	CHost * pHostTo = m_RunParam.m_pEngine->GetRoadNet()->m_allHosts.GetAt(nId);
	SIM_TIME lnTimeOut = m_RunParam.m_pEngine->GetSimTime() + nTimeOut * 1000;
	CRoutingDataEnc encData;
	encData.SetValue(pHostFrom, pHostTo, lnTimeOut);
	m_RunParam.m_pHost->m_pProtocol->SendPackage(encData);
}
