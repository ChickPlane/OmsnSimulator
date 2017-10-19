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
		sm_Records.RemoveKey(id);
		KillTimer(NULL, id);
		return;
	}
	else
	{
		if (!newRecord.m_bAlways)
		{
			sm_Records.RemoveKey(id);
			KillTimer(NULL, id);
		}
	}
	sm_Mutex.Unlock();
	newRecord.m_pCommonTimer->OnCommonTimer(newRecord.m_nIDEvent);
}

CMap<UINT_PTR, UINT_PTR, CCommonTimerRecord, CCommonTimerRecord&> CCommonTimer::sm_Records;

CMutex CCommonTimer::sm_Mutex;

int CCommonTimer::sm_LastError = 0;

UINT_PTR CCommonTimer::SetCommonTimer(UINT_PTR nIDEvent, UINT uElapse, BOOL bAlways)
{
	sm_nWorkingTimer++;
	UINT_PTR uTimerId = ::SetTimer(NULL, 0, uElapse, CCommonTimer::timerFun);
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
	newRecord.m_bAlways = bAlways;
	sm_Records[uTimerId] = newRecord;
	sm_Mutex.Unlock();
	return uTimerId;
}

BOOL CCommonTimer::KillCommonTimer(UINT_PTR nIDEvent)
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
			if (recordValue.m_nIDEvent == nIDEvent)
			{
				KillTimer(NULL, uKey);
				sm_Records.RemoveKey(uKey);
				sm_Mutex.Unlock();
				return TRUE;
			}
		}
	}
	sm_Mutex.Unlock();
	return FALSE;
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
			KillTimer(NULL, uKey);
			sm_Records.RemoveKey(uKey);
		}
	}
	sm_Mutex.Unlock();
}

int CCommonTimer::sm_nWorkingTimer = 0;
