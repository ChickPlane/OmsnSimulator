#pragma once

class CMsgShowRow
{
public:
	CMsgShowRow() {}
	CMsgShowRow(const CMsgShowRow & src) { *this = src; }
	CMsgShowRow & operator = (const CMsgShowRow & src);
	CString m_Item0;
	CString m_Item1;
	CString m_Item2;
	CArray<CString> m_OtherItems;

};

class CMsgShowInfo
{
public:
	CMsgShowInfo();
	virtual ~CMsgShowInfo();
	int m_nColCount;
	CList<CMsgShowRow> m_Rows;
};

