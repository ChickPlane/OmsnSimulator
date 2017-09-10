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
	CList<CString> allInfoString;
	m_pHost->m_pProtocol->GetInfoList(allInfoString);
	DeleteAllItems();
	POSITION pos = allInfoString.GetHeadPosition();
	CString strItemHead;
	int nLineNum = 0;
	while (pos)
	{
		CString strInfo = allInfoString.GetNext(pos);
		strItemHead.Format(_T("%07d"), nLineNum);
		InsertItem(nLineNum, strItemHead);
		SetItemText(nLineNum, PINFO_GRID_COL_INFO, strInfo);
		++nLineNum;
	}
}

BEGIN_MESSAGE_MAP(CListCtrlProcessInfo, CMFCListCtrl)
END_MESSAGE_MAP()



// CListCtrlProcessInfo 消息处理程序


