#include "stdafx.h"
#include "RoutingProtocolAptCard.h"
#include "RoutingProcessAptCard.h"


CRoutingProtocolAptCard::CRoutingProtocolAptCard()
{
	CRoutingProcessAptCard * pAptCardProcess = new CRoutingProcessAptCard();
	m_nAptCardProcessId = AddProcess(pAptCardProcess);
}

CRoutingProtocolAptCard::~CRoutingProtocolAptCard()
{
}

void CRoutingProtocolAptCard::OnReceivedMsg(const CYell * pMsg)
{
	m_Processes[m_nAptCardProcessId]->OnReceivedMsg(pMsg);
}

void CRoutingProtocolAptCard::CreateQueryMission(const CQueryMission * pMission)
{
	m_Processes[m_nAptCardProcessId]->GenerateMission(pMission);
}

void CRoutingProtocolAptCard::SetParameters(int nK, int nSeg, int nCopyCount, double fTrust, SIM_TIME lnAcTimeout)
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	pAptCardProcess->SetParameters(nK, nSeg, nCopyCount, fTrust, lnAcTimeout);
}

COLORREF CRoutingProtocolAptCard::GetInportantLevel() const
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	return pAptCardProcess->GetInportantLevel();
}

int CRoutingProtocolAptCard::GetInfoList(CMsgShowInfo & allMessages) const
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	pAptCardProcess->GetInfoList(allMessages);
	return 0;
}

void CRoutingProtocolAptCard::Turn(BOOL bOn)
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	pAptCardProcess->TurnOn();
}

int CRoutingProtocolAptCard::GetDebugNumber(int nParam)
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	return pAptCardProcess->GetCreatedCount();
}

CString CRoutingProtocolAptCard::GetDebugString() const
{
	CRoutingProcessAptCard * pAptCardProcess = (CRoutingProcessAptCard*)m_Processes[m_nAptCardProcessId];
	return pAptCardProcess->GetAgencyListString();
}
