// ListCtrlProcessInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "ListCtrlProcessInfo.h"
#include "Host.h"
#include "HostEngine.h"


// CListCtrlProcessInfo

IMPLEMENT_DYNAMIC(CListCtrlProcessInfo, CMFCListCtrl)

CListCtrlProcessInfo::CListCtrlProcessInfo()
{

}

CListCtrlProcessInfo::~CListCtrlProcessInfo()
{
}

void CListCtrlProcessInfo::Init(CHost * pHost, CHostEngine * pEngine)
{
	m_pHost = pHost;
	m_pEngine = pEngine;
}

void CListCtrlProcessInfo::ShowAllMsgs()
{
	if (!m_pHost || !m_pEngine)
	{
		return;
	}
	CMsgShowInfo allInfo;
	m_pHost->m_pProtocol->GetInfoList(allInfo);
	DeleteAllItems();
	POSITION pos = allInfo.m_Rows.GetHeadPosition();
	CString strItemHead;
	int nLineNum = 0;
	while (pos)
	{
		strItemHead.Format(_T("%07d"), nLineNum);
		InsertItem(nLineNum, strItemHead);

		CString strInfo = allInfo.m_Rows.GetNext(pos).m_Item0;
		SetItemText(nLineNum, PINFO_GRID_COL_INFO, strInfo);
		++nLineNum;
	}
}

BEGIN_MESSAGE_MAP(CListCtrlProcessInfo, CMFCListCtrl)
END_MESSAGE_MAP()



// CListCtrlProcessInfo 消息处理程序


