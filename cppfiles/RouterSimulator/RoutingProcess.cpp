#include "stdafx.h"
#include "RoutingProcess.h"
#include "HostEngine.h"

#include "RoutingProtocol.h"
#include "Host.h"
#include "Yell.h"


CRoutingProcess::CRoutingProcess()
	: m_pHost(NULL)
	, m_pProtocol(NULL)
	, m_nProcessID(-1)
{
}


CRoutingProcess::~CRoutingProcess()
{
}

void CRoutingProcess::SetEnvironment(CHost * pHost, CHostEngine * pEngine)
{
	m_pHost = pHost;
	SetEngine(pEngine);
}

void CRoutingProcess::TransmitMessage(CRoutingProtocol * pTo, CYell * pMsg)
{
	if (m_pProtocol)
	{
		pMsg->m_pSender = m_pProtocol;
		pMsg->m_pRecver = pTo;
		m_pProtocol->TransmitMessage(this, pTo, pMsg);
	}
	else
	{
		ASSERT(0);
	}
}

int CRoutingProcess::GetCarryingMessages(CMsgShowInfo & allMessages) const
{
	return 0;
}

int CRoutingProcess::GetInfoList(CList<CString> & ret)
{
	return  0;
}

void CRoutingProcess::WriteLog(const CString & strLog)
{
	CString strWriteDown = _T("[Process] ") + strLog;
	EngineWriteLog(strWriteDown);
	OutputDebugString(_T("\n") + strWriteDown);
}