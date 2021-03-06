#include "stdafx.h"
#include "RoutingProcessHello.h"
#include "Yell.h"
#include "Sentence.h"
#include "RoutingProtocol.h"


CRoutingProcessHello::CRoutingProcessHello()
	: m_bIsSearching(FALSE)
	, m_bSendHello(TRUE)
{
}


CRoutingProcessHello::~CRoutingProcessHello()
{
}

void CRoutingProcessHello::SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol)
{
	CRoutingProcess::SetBasicParameters(nProcessID, pProtocol);
}

void CRoutingProcessHello::OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList)
{
	switch (pPkg->m_nSentenceType)
	{
	case SENTENCE_TYPE_HELLO:
	{
		m_pUser->OnHearHelloFromOthers(this, (CPkgHello *)pPkg);
		CPkgAck * pAck = m_pUser->GetAckPackage(this, pPkg->m_pSender);
		MarkProcessIdToSentences(pAck);
		SendingList.AddTail(pAck);
		break;
	}
	case SENTENCE_TYPE_ACK:
	{
		m_pUser->OnBuiltConnectWithOthers(this, (CPkgAck *)pPkg);
		CPkgAck * pAck = m_pUser->GetOkPackage(this, pPkg->m_pSender);
		MarkProcessIdToSentences(pAck);
		SendingList.AddTail(pAck);
		break;
	}
	case SENTENCE_TYPE_OK:
	{
		m_pUser->OnBuiltConnectWithOthers(this, (CPkgAck *)pPkg);
		break;
	}
	default:
	{
		ASSERT(0);
	}
	}
}

void CRoutingProcessHello::OnSomeoneNearby(BOOL bAnyOneNearby, BOOL bDifferentFromPrev)
{
	if (m_bSendHello && bAnyOneNearby)
	{
		SendHelloPackage();
	}
}

void CRoutingProcessHello::StartWork(BOOL bStart)
{
	StartSearhing();
}

void CRoutingProcessHello::StartSearhing()
{
	if (m_bIsSearching == FALSE)
	{
		m_bIsSearching = TRUE;
	}
}

void CRoutingProcessHello::SendHelloPackage()
{
	CPkgHello * pHello = m_pUser->GetHelloPackage(this);
	if (pHello)
	{
		CYell * pNewYell = new CYell();
		pNewYell->SetSentenceLength(1);
		pNewYell->m_ppSentences[0] = pHello;
		TransmitMessage(NULL, pNewYell);
	}
	else
	{
		ASSERT(0);
	}
}

SIM_TIME CRoutingProcessHello::m_lnSearchInterval = 10000;

void CRoutingProcessHelloUser::OnHearHelloFromOthers(CRoutingProcessHello * pCallBy, const CPkgHello * pPkg)
{

}

CPkgHello * CRoutingProcessHelloUser::GetHelloPackage(CRoutingProcessHello * pCallBy)
{
	return new CPkgHello();
}

CPkgAck * CRoutingProcessHelloUser::GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo)
{
	CPkgAck * pRet = new CPkgAck();
	pRet->m_pSpeakTo = pTo;
	return pRet;
}

CPkgAck * CRoutingProcessHelloUser::GetOkPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo)
{
	CPkgAck * pRet = GetAckPackage(pCallBy, pTo);
	pRet->SetAsOkPackage();
	return pRet;
}
