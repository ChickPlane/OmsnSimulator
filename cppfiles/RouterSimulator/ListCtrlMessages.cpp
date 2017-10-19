// ListCtrlMessages.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "ListCtrlMessages.h"
#include "Host.h"
#include "HostEngine.h"
#include "MsgShowInfo.h"


// CListCtrlMessages

IMPLEMENT_DYNAMIC(CListCtrlMessages, CMFCListCtrl)

CListCtrlMessages::CListCtrlMessages()
	: m_pHost(NULL)
	, m_pEngine(NULL)
{

}

CListCtrlMessages::~CListCtrlMessages()
{
}


void CListCtrlMessages::Init(CHost * pHost, CHostEngine * pEngine)
{
	m_pHost = pHost;
	m_pEngine = pEngine;
}

void CListCtrlMessages::ShowAllMsgs()
{
	if (!m_pHost || !m_pEngine)
	{
		return;
	}
	CMsgShowInfo allMessages;
	m_pHost->GetAllCarryingMessages(allMessages);
	DeleteAllItems();
	POSITION pos = allMessages.m_Rows.GetHeadPosition();
	CString strItemHead;
	CString strItem;
	int nLineNum = 0;
	while (pos)
	{
		CMsgShowRow & tmpRow = allMessages.m_Rows.GetNext(pos);
		strItemHead.Format(_T("%07d"), nLineNum);
		InsertItem(nLineNum, strItemHead);
		SetItemText(nLineNum, MSG_GRID_COL_ID, tmpRow.m_Item0);
		SetItemText(nLineNum, MSG_GRID_COL_REMAINED, tmpRow.m_Item1);
		SetItemText(nLineNum, MSG_GRID_COL_HOPS, tmpRow.m_Item2);
		++nLineNum;
	}
}

BEGIN_MESSAGE_MAP(CListCtrlMessages, CMFCListCtrl)
END_MESSAGE_MAP()



// CListCtrlMessages 消息处理程序


