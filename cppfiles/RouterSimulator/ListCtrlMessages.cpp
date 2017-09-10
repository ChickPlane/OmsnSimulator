// ListCtrlMessages.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "ListCtrlMessages.h"
#include "RoutingMsg.h"
#include "Host.h"
#include "HostEngine.h"


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
	CList<CRoutingDataEnc> allMessages;
	m_pHost->GetAllCarryingMessages(allMessages);
	DeleteAllItems();
	POSITION pos = allMessages.GetHeadPosition();
	CString strItemHead;
	CString strItem;
	int nLineNum = 0;
	while (pos)
	{
		CRoutingDataEnc encData = allMessages.GetNext(pos);
		strItemHead.Format(_T("%07d"), nLineNum);
		InsertItem(nLineNum, strItemHead);
		strItem.Format(_T("%d"), encData.ForceGetDataId(NULL));
		SetItemText(nLineNum, MSG_GRID_COL_ID, strItem);
		strItem.Format(_T("%d"), (int)(encData.ForceGetTimeOut(NULL) - m_pEngine->GetSimTime()) / 1000);
		SetItemText(nLineNum, MSG_GRID_COL_REMAINED, strItem);
		strItem.Format(_T("%2f"), encData.m_Statistic.GetHopCount());
		SetItemText(nLineNum, MSG_GRID_COL_HOPS, strItem);
		++nLineNum;
	}
}

BEGIN_MESSAGE_MAP(CListCtrlMessages, CMFCListCtrl)
END_MESSAGE_MAP()



// CListCtrlMessages 消息处理程序


