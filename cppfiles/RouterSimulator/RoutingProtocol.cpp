#include "stdafx.h"
#include "RoutingProtocol.h"
#include "HostEngine.h"
#include "Host.h"
#include "RoutingProcess.h"
#include "Yell.h"
#include "Sentence.h"


CRoutingProtocol::CRoutingProtocol()
	: m_fCommunicationRadius(0)
	, m_nSessionRecordEntrySize(0)
	, m_nProtocolRecordEntrySize(0)
	, m_nForwardBoundary(0)
{
}

CRoutingProtocol::~CRoutingProtocol()
{
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		delete m_Processes[i];
	}
}

double CRoutingProtocol::GetCommunicateRadius() const
{
	return m_fCommunicationRadius;
}

void CRoutingProtocol::OnReceivedMsg(const CYell * pMsg)
{
	if (pMsg->m_pSender == this)
	{
		return;
	}
	if (pMsg->m_pRecver != NULL && pMsg->m_pRecver != this)
	{
		return;
	}
	CList<CSentence*> answer;
	for (int i = 0; i < pMsg->m_nSentenceCount; ++i)
	{
		CSentence * pSentence = pMsg->m_ppSentences[i];
		CRoutingProcess * pProcess = m_Processes[pSentence->m_nProcessID];
		pProcess->OnReceivePkgFromNetwork(pSentence, answer);
	}
	if (answer.GetSize() > 0)
	{
		CYell * pNewYell = new CYell();
		CRoutingProtocol * pTo = pNewYell->SetSentences(answer);
		TransmitMessage(pTo, pNewYell);
	}
}

void CRoutingProtocol::OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev)
{

}

void CRoutingProtocol::SetEnvironment(CHost * pHost, CHostEngine * pEngine)
{
	m_pHost = pHost;
	SetEngine(pEngine);
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		m_Processes[i]->SetEngine(pEngine);
	}

	CStatisticSummary & summary = GetEngine()->GetSummary();
	if (summary.m_RecentProtocolTag.m_ProtocolRecords.GetSize() != m_nProtocolRecordEntrySize)
	{
		summary.m_RecentProtocolTag.m_ProtocolRecords.SetSize(m_nProtocolRecordEntrySize);
	}

	int nTotalSessionStatisticNumber = GetTotalSessionStatisticEntryNumber();
	if (summary.m_RecentSessionTag.m_SessionRecords.GetSize() != nTotalSessionStatisticNumber)
	{
		summary.m_RecentSessionTag.m_SessionRecords.SetSize(nTotalSessionStatisticNumber);
	}
}

void CRoutingProtocol::SetCommunicateRadius(double fCommunicationRadius)
{
	m_fCommunicationRadius = fCommunicationRadius;
}

void CRoutingProtocol::OnEngineTimer(int nCommandId)
{

}

void CRoutingProtocol::GetAllCarryingMessages(CMsgShowInfo & allMessages) const
{
	allMessages.m_Rows.RemoveAll();
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		m_Processes[i]->GetCarryingMessages(allMessages);
	}
}

void CRoutingProtocol::OnDelivered(const CQueryMission * pMission)
{
	if (!GetHost()->IsReceivedPackage(pMission))
	{
		GetHost()->OnPackageArrived(pMission);
	}
}

CHost * CRoutingProtocol::GetHost() const
{
	return m_pHost;
}

int CRoutingProtocol::GetHostId() const
{
	return GetHost()->m_nId;
}

CDoublePoint CRoutingProtocol::GetHostPostion(SIM_TIME lnTime) const
{
	return GetHost()->GetPosition(lnTime);
}

int CRoutingProtocol::AddProcess(CRoutingProcess * pProcess)
{
	int nProcessId = m_Processes.GetSize();
	pProcess->SetBasicParameters(nProcessId, this);
	m_Processes.Add(pProcess);
	return nProcessId;
}

int CRoutingProtocol::GetInfoList(CMsgShowInfo & allMessages) const
{
	return 0;
}

void CRoutingProtocol::Turn(BOOL bOn)
{
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		m_Processes[i]->StartWork(TRUE);
	}
}

void CRoutingProtocol::TransmitMessage(CRoutingProtocol * pTo, CYell * pMsg)
{
	if (GetEngine())
	{
		pMsg->m_pSender = this;
		pMsg->m_pRecver = pTo;
		for (int i = 0; i < pMsg->m_nSentenceCount; ++i)
		{
			pMsg->m_ppSentences[i]->m_pSender = this;
		}
		GetEngine()->TransmitMessage(this, pTo, pMsg);
	}
	else
	{
		ASSERT(0);
	}
}

void CRoutingProtocol::TransmitSingleSentence(CSentence * pSentence)
{
	if (GetEngine())
	{
		pSentence->m_pSender = this;

		CYell * pNewYell = new CYell();
		pNewYell->SetSentenceLength(1);
		pNewYell->m_ppSentences[0] = pSentence;
		pNewYell->m_pSender = this;
		pNewYell->m_pRecver = pSentence->m_pSpeakTo;
		GetEngine()->TransmitMessage(this, pSentence->m_pSpeakTo, pNewYell);
	}
	else
	{
		ASSERT(0);
	}
}

int CRoutingProtocol::GetProcessId(CRoutingProcess * pProcess)
{
	for (int i = 0; i < m_Processes.GetSize(); ++i)
	{
		if (m_Processes[i] == pProcess)
		{
			return i;
		}
	}
	return INVALID_PROCESS_ID;
}


int CRoutingProtocol::GetCarryingPkgNumber(int nParam)
{
	switch (nParam)
	{
	case PROTOCOL_PKG_TYPE_SUPPLY:
	{
		return 0;
	}
	case PROTOCOL_PKG_TYPE_QUERY:
	{
		return 0;
	}
	case PROTOCOL_PKG_TYPE_REPLY:
	{
		return 0;
	}
	}
}

void CRoutingProtocol::WriteLog(const CString & strLog)
{
#ifdef DEBUG
	CString strWriteDown = m_strLogPrefix + _T(" ") + strLog;
	EngineWriteLog(strWriteDown);
	OutputDebugString(_T("\n") + strWriteDown);
#endif
}

BOOL CRoutingProtocol::gm_bEnableLbsp = TRUE;

BOOL CRoutingProtocol::SetSissionRecord(int nSessionId, int nEventId)
{
	CTestRecord * pRecord = GetSessionRecord(nSessionId);
	if (pRecord->m_pMilestoneTime[nEventId] == INVALID_SIMTIME)
	{
		pRecord->m_pMilestoneTime[nEventId] = GetSimTime();
		OnSessionRecordChanged();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CRoutingProtocol::SetSissionForwardNumber(int nSessionId, int nForwardNumber)
{
	CTestRecord * pRecord = GetSessionRecord(nSessionId);
	if (pRecord->m_nForwardTimes == 0)
	{
		pRecord->m_nForwardTimes = nForwardNumber;
		OnSessionRecordChanged();
		return TRUE;
	}
	return FALSE;
}

void CRoutingProtocol::OnSessionRecordChanged()
{
	CStatisticSummary & summary = GetEngine()->GetSummary();
	int nTotalSessionStatisticNumber = GetTotalSessionStatisticEntryNumber();
	if (summary.m_RecentSessionTag.m_SessionRecords.GetSize() != nTotalSessionStatisticNumber)
	{
		summary.m_RecentSessionTag.m_SessionRecords.SetSize(nTotalSessionStatisticNumber);
	}

	for (int i = 0; i < nTotalSessionStatisticNumber; ++i)
	{
		summary.m_RecentSessionTag.m_SessionRecords[i] = 0;
	}

	int nForwardRecordCount = 0;
	int nForwardCountIndex = 0;
	POSITION pos = gm_allSessionRecords.GetStartPosition();
	while (pos)
	{
		CTestRecord * pRecord = NULL;
		int nId = 0;
		gm_allSessionRecords.GetNextAssoc(pos, nId, pRecord);
		int i = 0;
		for (i = 0; i < m_nSessionRecordEntrySize; ++i)
		{
			if (pRecord->m_pMilestoneTime[i] > INVALID_SIMTIME)
			{
				summary.m_RecentSessionTag.m_SessionRecords[i]++;
			}
		}
		nForwardCountIndex = i;
		if (pRecord->m_nForwardTimes > 0)
		{
			nForwardRecordCount++;
			summary.m_RecentSessionTag.m_SessionRecords[nForwardCountIndex] += pRecord->m_nForwardTimes;
		}
	}
	if (nForwardRecordCount > 0)
	{
		summary.m_RecentSessionTag.m_SessionRecords[m_nSessionRecordEntrySize + ENGINE_RECORD_FORWARD_TIMES] /= nForwardRecordCount;
	}
	GetEngine()->ChangeSummary(TRUE);
}

void CRoutingProtocol::OnProtocolRecordChanged()
{
	CStatisticSummary & summary = GetEngine()->GetSummary();
	if (summary.m_RecentProtocolTag.m_ProtocolRecords.GetSize() != m_nProtocolRecordEntrySize)
	{
		ASSERT(0);
		summary.m_RecentProtocolTag.m_ProtocolRecords.SetSize(m_nProtocolRecordEntrySize);
	}

	for (int i = 0; i < m_nProtocolRecordEntrySize; ++i)
	{
		summary.m_RecentProtocolTag.m_ProtocolRecords[i] = 0;
	}

	POSITION pos = gm_allProtocolRecords.GetStartPosition();
	while (pos)
	{
		const CRoutingProtocol * pProtocol = NULL;
		CProtocolRecord * pProtocolRecord = NULL;
		gm_allProtocolRecords.GetNextAssoc(pos, pProtocol, pProtocolRecord);
		int i = 0;
		for (i = 0; i < m_nProtocolRecordEntrySize; ++i)
		{
			summary.m_RecentProtocolTag.m_ProtocolRecords[i] += pProtocolRecord->m_pData[i];
		}
	}
	
	if (gm_allProtocolRecords.GetSize() > 1)
	{
		for (int i = 0; i < m_nProtocolRecordEntrySize; ++i)
		{
			summary.m_RecentProtocolTag.m_ProtocolRecords[i] /= gm_allProtocolRecords.GetSize();
		}
	}
	GetEngine()->ChangeSummary(TRUE);
}

CTestRecord * CRoutingProtocol::GetSessionRecord(int nSessionId)
{
	CTestRecord * pRecord = NULL;
	if (!gm_allSessionRecords.Lookup(nSessionId, pRecord))
	{
		pRecord = new CTestRecord(m_nSessionRecordEntrySize);
		gm_allSessionRecords[nSessionId] = pRecord;
	}
	return pRecord;
}

CProtocolRecord * CRoutingProtocol::GetProtocolRecord() const
{
	CProtocolRecord * pFindResult = NULL;
	if (gm_allProtocolRecords.Lookup(this, pFindResult))
	{
		return pFindResult;
	}
	pFindResult = new CProtocolRecord(m_nProtocolRecordEntrySize);
	gm_allProtocolRecords[this] = pFindResult;
	return pFindResult;
}

double CRoutingProtocol::GetProtocolRecordValue(int nEventId) const
{
	return GetProtocolRecord()->m_pData[nEventId];
}

void CRoutingProtocol::SetProtocolRecordValue(int nEventId, double fValue)
{
	GetProtocolRecord()->m_pData[nEventId] = fValue;
	OnProtocolRecordChanged();
}

CMap<int, int, CTestRecord *, CTestRecord *> CRoutingProtocol::gm_allSessionRecords;

CMap<const CRoutingProtocol *, const CRoutingProtocol *, CProtocolRecord *, CProtocolRecord *> CRoutingProtocol::gm_allProtocolRecords;

CString CRoutingProtocol::GetDebugString() const
{
	return _T("");
}

