#pragma once

class CHost;
class CHostEngine;

enum
{
	MSG_GRID_COL_ID = 0,
	MSG_GRID_COL_START_TIME,
	MSG_GRID_COL_REMAINED,
	MSG_GRID_COL_HOPS,
};

// CListCtrlMessages

class CListCtrlMessages : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CListCtrlMessages)

public:
	CListCtrlMessages();
	virtual ~CListCtrlMessages();
	void Init(CHost * pHost, CHostEngine * pEngine);
	void ShowAllMsgs();

	CHost * m_pHost;
	CHostEngine * m_pEngine;

protected:
	DECLARE_MESSAGE_MAP()
};


