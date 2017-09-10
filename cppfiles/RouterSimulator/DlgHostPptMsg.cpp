// DlgHostPptMsg.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "DlgHostPptMsg.h"
#include "afxdialogex.h"
#include "ListCtrlMessages.h"
#include "DlgHostProperty.h"
#include "ListCtrlProcessInfo.h"


// CDlgHostPptMsg 对话框

IMPLEMENT_DYNAMIC(CDlgHostPptMsg, CDialogEx)

CDlgHostPptMsg::CDlgHostPptMsg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_HOST_INFO_MSG, pParent)
	, m_pGrid(NULL)
	, m_pProcessInfo(NULL)
{

}

CDlgHostPptMsg::~CDlgHostPptMsg()
{
}

void CDlgHostPptMsg::ResetData(CHostPropertyParam * pParam)
{
	m_pGrid->Init(pParam->m_pHost, pParam->m_pEngine);
	m_pGrid->ShowAllMsgs();
	m_pProcessInfo->Init(pParam->m_pHost, pParam->m_pEngine);
	m_pProcessInfo->ShowAllMsgs();
}

void CDlgHostPptMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHostPptMsg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgHostPptMsg 消息处理程序


BOOL CDlgHostPptMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rctClient;
	GetClientRect(&rctClient);
	rctClient.DeflateRect(10, 10, 10, 10);
	m_pGrid = new CListCtrlMessages();
	//m_pGrid->Create(WS_BORDER | WS_DLGFRAME | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, rctClient, this, 12345);
	
	m_pGrid->Create(WS_BORDER | LVS_REPORT | LVS_ALIGNLEFT, rctClient, this, 0x285);
	m_pGrid->ShowWindow(SW_SHOW);
	m_pGrid->InsertColumn(0, _T("Msg ID"), LVCFMT_LEFT, 100);
	m_pGrid->InsertColumn(1, _T("Sent Time"), LVCFMT_LEFT, 100);
	m_pGrid->InsertColumn(2, _T("Remained"), LVCFMT_LEFT, 100);
	m_pGrid->InsertColumn(3, _T("hops"), LVCFMT_LEFT, 100);

	m_pProcessInfo = new CListCtrlProcessInfo();
	//m_pGrid->Create(WS_BORDER | WS_DLGFRAME | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, rctClient, this, 12345);

	m_pProcessInfo->Create(WS_BORDER | LVS_REPORT | LVS_ALIGNLEFT, rctClient, this, 0x285);
	m_pProcessInfo->ShowWindow(SW_SHOW);
	m_pProcessInfo->InsertColumn(0, _T("Process Info"), LVCFMT_LEFT, 300);

	SetTimer(DLG_HOST_PPT_TIMER_ID, 500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgHostPptMsg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (IsWindow(m_pGrid->GetSafeHwnd()))
	{
		CRect rctClient;
		GetClientRect(&rctClient);
		rctClient.bottom -= rctClient.Height() / 2;
		rctClient.DeflateRect(10, 10, 10, 5);
		m_pGrid->MoveWindow(rctClient);
	}
	if (IsWindow(m_pProcessInfo->GetSafeHwnd()))
	{
		CRect rctClient;
		GetClientRect(&rctClient);
		rctClient.top += rctClient.Height() / 2;
		rctClient.DeflateRect(10, 5, 10, 10);
		m_pProcessInfo->MoveWindow(rctClient);
	}
	// TODO: 在此处添加消息处理程序代码
}


void CDlgHostPptMsg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case DLG_HOST_PPT_TIMER_ID:
	{
		m_pGrid->ShowAllMsgs();
		m_pProcessInfo->ShowAllMsgs();
		break;
	}
	}

	CDialogEx::OnTimer(nIDEvent);
}
