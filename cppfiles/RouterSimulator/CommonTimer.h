#pragma once
#include "CommonTimerRecord.h"

class CCommonTimer
{
public:
	CCommonTimer();
	virtual ~CCommonTimer();

	static VOID CALLBACK timerFun(HWND wnd, UINT msg, UINT_PTR id, DWORD d);
	static CMap<UINT_PTR, UINT_PTR, CCommonTimerRecord, CCommonTimerRecord&> sm_Records;
	static CMutex sm_Mutex;
	static int sm_LastError;
	UINT_PTR SetCommonTimer(UINT_PTR nIDEvent, UINT uElapse);
	virtual void OnCommonTimer(UINT_PTR nIDEvent) = 0;
protected:
	void CleanSelf();

public:
	static int sm_nWorkingTimer;
};

