// HostStateWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "HostStateWnd.h"
#include "DlgHostProperty.h"


// CHostStateWnd

IMPLEMENT_DYNAMIC(CHostStateWnd, CDockablePane)

CHostStateWnd::CHostStateWnd()
	: m_pDlgProperty(NULL)
{

}

CHostStateWnd::~CHostStateWnd()
{
}


void CHostStateWnd::AdjustLayout()
{
	if (m_pDlgProperty)
	{
		CRect rctClient;
		GetClientRect(&rctClient);
		m_pDlgProperty->SetWindowPos(NULL, rctClient.left, rctClient.top, rctClient.Width(), rctClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

BEGIN_MESSAGE_MAP(CHostStateWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CHostStateWnd 消息处理程序




int CHostStateWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_pDlgProperty = new CDlgHostProperty();
	m_pDlgProperty->Create(IDD_DIALOG_HOST_INFO, this);
	m_pDlgProperty->ShowWindow(SW_SHOW);

	return 0;
}


void CHostStateWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	AdjustLayout();
}
