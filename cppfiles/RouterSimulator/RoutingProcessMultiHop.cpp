#include "stdafx.h"
#include "RoutingProcessMultiHop.h"
#include "RoutingProtocol.h"
#include "Yell.h"


CPkgMultiHop * CRoutingProcessMultiHopUser::GetMultiHopDataCopy(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg)
{
	return new CPkgMultiHop(*pPkg);
}


CRoutingProcessMultiHop::CRoutingProcessMultiHop()
{
}


CRoutingProcessMultiHop::~CRoutingProcessMultiHop()
{
}

void CRoutingProcessMultiHop::SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol)
{
	CRoutingProcess::SetBasicParameters(nProcessID, pProtocol);
}

void CRoutingProcessMultiHop::OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList)
{
	int nSentenceType = pPkg->m_nSentenceType;
	switch (nSentenceType)
	{
	case MH_SENTENCE_TYPE_DATA:
	{
		OnReceivedDataPkg((const CPkgMultiHop*)pPkg);
		break;
	}
	default:
	{
		ASSERT(0);
	}
	}
}

void CRoutingProcessMultiHop::SendPkgToMultiHopHost(const CPkgMultiHop * pPkg)
{
	OnReceivedDataPkg(pPkg);
}

void CRoutingProcessMultiHop::OnReceivedDataPkg(const CPkgMultiHop * pPkg)
{
	if (pPkg->m_nToId == m_pProtocol->GetHostId())
	{
		m_pUser->OnMultiHopMsgArrived(this, pPkg);
		return;
	}
	CRoutingProtocol * pNextHop = m_pUser->GetNextHop(this, pPkg->m_nToId);
	if (pNextHop == NULL)
	{
		ASSERT(0);
		return;
	}
	CPkgMultiHop * pNewPkg = m_pUser->GetMultiHopDataCopy(this, pPkg);
	pNewPkg->m_pSpeakTo = pNextHop;
	MarkProcessIdToSentences(pNewPkg);

	CYell * pNewYell = new CYell();
	pNewYell->SetSentenceLength(1);
	pNewYell->m_ppSentences[0] = pNewPkg;
	TransmitMessage(pNextHop, pNewYell);

}

double CRoutingProcessMultiHop::gm_fObfuscationRadius;
