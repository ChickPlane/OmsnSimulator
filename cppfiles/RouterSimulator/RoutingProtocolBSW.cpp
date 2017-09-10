#include "stdafx.h"
#include "RoutingProtocolBSW.h"
#include "RoutingMsgBSW.h"
#include "Host.h"
#include "RoutingProtocol.h"
#include "HostEngine.h"
#include "RoutingProcessBSW.h"


CRoutingProtocolBSW::CRoutingProtocolBSW()
{
	CRoutingProcessBSW * pBswProcess = new CRoutingProcessBSW();
	m_nBswProcessId = AddProcess(pBswProcess);
}


CRoutingProtocolBSW::~CRoutingProtocolBSW()
{
}

void CRoutingProtocolBSW::OnReceivedMsg(const CRoutingMsg * pMsg)
{
	if (!pMsg->IsBelongTo(this))
	{
		return;
	}
	m_Processes[m_nBswProcessId]->OnReceivedMsg(pMsg);
}

void CRoutingProtocolBSW::SendPackage(const CRoutingDataEnc & encData)
{
	CRoutingDataEnc originalData(encData);
	RecordNewPackage(originalData);
	m_Processes[m_nBswProcessId]->GenerateMission(originalData);
}

void CRoutingProtocolBSW::SetCopyCount(int nCopyCount)
{
	CRoutingProcessBSW * pBswProcess = (CRoutingProcessBSW*)m_Processes[m_nBswProcessId];
	pBswProcess->SetCopyCount(nCopyCount);
}

int CRoutingProtocolBSW::GetInportantLevel() const
{
	CRoutingProcessBSW * pBswProcess = (CRoutingProcessBSW*)m_Processes[m_nBswProcessId];
	if (pBswProcess->m_DataList.GetSize() > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
