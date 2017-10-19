#pragma once

class CCommonTimer;

class CCommonTimerRecord
{
public:
	CCommonTimerRecord();
	CCommonTimerRecord(const CCommonTimerRecord & src);
	~CCommonTimerRecord();

	CCommonTimerRecord & operator = (const CCommonTimerRecord & src);

	UINT_PTR m_nIDEvent;
	CCommonTimer * m_pCommonTimer;
	BOOL m_bAlways;
};

