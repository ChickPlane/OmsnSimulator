#include "stdafx.h"
#include "CommonTimer.h"



CCommonTimer::CCommonTimer()
{
}


CCommonTimer::~CCommonTimer()
{
	CleanSelf();
}

VOID CALLBACK CCommonTimer::timerFun(HWND wnd, UINT msg, UINT_PTR id, DWORD d)
{
	--sm_nWorkingTimer;
	CCommonTimerRecord newRecord;

	sm_Mutex.Lock();
	if (!sm_Records.Lookup(id, newRecord))
	{
		ASSERT(0);
		sm_LastError = 123;
		return;
	}
	sm_Records.RemoveKey(id);
	sm_Mutex.Unlock();
	newRecord.m_pCommonTimer->OnCommonTimer(newRecord.m_nIDEvent);
	KillTimer(NULL, id);
}

CMap<UINT_PTR, UINT_PTR, CCommonTimerRecord, CCommonTimerRecord&> CCommonTimer::sm_Records;

CMutex CCommonTimer::sm_Mutex;

int CCommonTimer::sm_LastError = 0;

UINT_PTR CCommonTimer::SetCommonTimer(UINT_PTR nIDEvent, UINT uElapse)
{
	sm_nWorkingTimer++;
	UINT_PTR uTimerId = SetTimer(NULL, nIDEvent, uElapse, CCommonTimer::timerFun);
	if (uTimerId == 0)
	{
		ASSERT(0);
		return uTimerId;
	}
	CCommonTimerRecord newRecord;
	sm_Mutex.Lock();
	if (sm_Records.Lookup(uTimerId, newRecord))
	{
		ASSERT(0);
		return -123;
	}
	newRecord.m_nIDEvent = nIDEvent;
	newRecord.m_pCommonTimer = this;
	sm_Records[uTimerId] = newRecord;
	sm_Mutex.Unlock();
	return uTimerId;
}

void CCommonTimer::OnCommonTimer(UINT_PTR nIDEvent)
{

}

void CCommonTimer::CleanSelf()
{
	sm_Mutex.Lock();
	POSITION pos = sm_Records.GetStartPosition();
	UINT_PTR uKey = 0;
	CCommonTimerRecord recordValue;
	while (pos)
	{
		sm_Records.GetNextAssoc(pos, uKey, recordValue);
		if (recordValue.m_pCommonTimer == this)
		{
			sm_Records.RemoveKey(uKey);
		}
	}
	sm_Mutex.Unlock();
}

int CCommonTimer::sm_nWorkingTimer = 0;
