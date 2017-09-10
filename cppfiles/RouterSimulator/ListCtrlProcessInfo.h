#pragma once

class CHost;
class CHostEngine;

enum
{
	PINFO_GRID_COL_INFO = 0,
};

// CListCtrlProcessInfo

class CListCtrlProcessInfo : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CListCtrlProcessInfo)

public:
	CListCtrlProcessInfo();
	virtual ~CListCtrlProcessInfo();
	void Init(CHost * pHost, CHostEngine * pEngine);
	void ShowAllMsgs();

	CHost * m_pHost;
	CHostEngine * m_pEngine;

protected:
	DECLARE_MESSAGE_MAP()
};


