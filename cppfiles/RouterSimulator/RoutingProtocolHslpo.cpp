#include "stdafx.h"
#include "RoutingProtocolHslpo.h"
#include "RoutingProcess.h"
#include "RoutingProcessHslpo.h"



CRoutingProtocolHslpo::CRoutingProtocolHslpo()
{
	CRoutingProcessHslpo * pHslpoProcess = new CRoutingProcessHslpo();
	m_nHslpoProcessId = AddProcess(pHslpoProcess);
}


CRoutingProtocolHslpo::~CRoutingProtocolHslpo()
{
}

void CRoutingProtocolHslpo::OnReceivedMsg(const CRoutingMsg * pMsg)
{
	if (!pMsg->IsBelongTo(this))
	{
		return;
	}
	m_Processes[m_nHslpoProcessId]->OnReceivedMsg(pMsg);
}

void CRoutingProtocolHslpo::SendPackage(const CRoutingDataEnc & encData)
{
	CRoutingDataEnc originalData(encData);
	RecordNewPackage(originalData);
	m_Processes[m_nHslpoProcessId]->GenerateMission(originalData);
}

void CRoutingProtocolHslpo::SetPrivacyParam(int nK, double fHigh, double fLow)
{
	((CRoutingProcessHslpo *)m_Processes[m_nHslpoProcessId])->SetPrivacyParam(nK, fHigh, fLow);
}

void CRoutingProtocolHslpo::SetCopyCount(int nCopyCount)
{
	CRoutingProcessHslpo * pBswProcess = (CRoutingProcessHslpo*)m_Processes[m_nHslpoProcessId];
	pBswProcess->SetCopyCount(nCopyCount);
}

int CRoutingProtocolHslpo::GetInportantLevel() const
{
	CRoutingProcessHslpo * pBswProcess = (CRoutingProcessHslpo*)m_Processes[m_nHslpoProcessId];
	if (pBswProcess->m_DataList.GetSize() == 0)
	{
		return 0;
	}
	else if (0 < pBswProcess->GetAnonymityPackageCount())
	{
		return 2;
	}
	else
	{
		return 1;
	}
}
