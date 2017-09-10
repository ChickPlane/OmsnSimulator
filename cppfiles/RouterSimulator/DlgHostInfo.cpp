// DlgHostInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "DlgHostInfo.h"
#include "afxdialogex.h"


// CDlgHostInfo 对话框

IMPLEMENT_DYNAMIC(CDlgHostInfo, CDialogEx)

CDlgHostInfo::CDlgHostInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_pHost(NULL)
	, m_pRoadNet(NULL)
{

}

CDlgHostInfo::~CDlgHostInfo()
{
}

void CDlgHostInfo::SetData(CHost * pHost, CRoadNet * pRoadNet)
{
	m_pHost = pHost;
	m_pRoadNet = pRoadNet;
	m_pHost->GetInfo(m_Info);
}

void CDlgHostInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEditHostId);
	DDX_Control(pDX, IDC_EDIT_TARGETNUM, m_ctrlEditTargetNum);
	DDX_Control(pDX, IDC_LIST1, m_ctrlMsgs);
	DDX_Control(pDX, IDC_CHECK_FOLLOW, m_ctrlCheckFollow);
}


BEGIN_MESSAGE_MAP(CDlgHostInfo, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgHostInfo::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDlgHostInfo::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDOK, &CDlgHostInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_FOLLOW, &CDlgHostInfo::OnBnClickedCheckFollow)
END_MESSAGE_MAP()


// CDlgHostInfo 消息处理程序


void CDlgHostInfo::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTargetNum;
	m_ctrlEditTargetNum.GetWindowText(strTargetNum);
	int nTargetId = _ttoi(strTargetNum.GetBuffer(0));

	CHost * pHostFrom = m_pHost;
	CHost * pHostTo = m_pRoadNet->m_allHosts.GetAt(nTargetId);
	SIM_TIME lnTimeOut = m_pHost->m_pProtocol->GetSimTime() + 1000000;
	CRoutingDataEnc encData;
	encData.SetValue(pHostFrom, pHostTo, lnTimeOut);
	m_pHost->m_pProtocol->SendPackage(encData);

	UpdateUi();
}


BOOL CDlgHostInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateUi();
	m_bFollow = true;
	m_ctrlCheckFollow.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgHostInfo::UpdateUi()
{
	m_pHost->GetInfo(m_Info);
	CString strSummary;
	strSummary.Format(_T("%d"), m_Info.m_nHostId);
	m_ctrlEditHostId.SetWindowText(strSummary);

	m_ctrlMsgs.ResetContent();
	POSITION pos = m_Info.m_protocolInfo.m_MsgInfos.GetHeadPosition();
	while (pos)
	{
		m_ctrlMsgs.AddString(m_Info.m_protocolInfo.m_MsgInfos.GetNext(pos).m_strSummary);
	}
}


void CDlgHostInfo::OnBnClickedButtonRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateUi();
}


void CDlgHostInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CDlgHostInfo::OnBnClickedCheckFollow()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCheck = m_ctrlCheckFollow.GetCheck();
	if (nCheck == BST_UNCHECKED)
	{
		m_bFollow = false;
	}
	else
	{
		m_bFollow = true;
	}
}
