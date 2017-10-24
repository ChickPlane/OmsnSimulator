#include "stdafx.h"
#include "RoutingProcessHello.h"
#include "Yell.h"
#include "Sentence.h"


CRoutingProcessHello::CRoutingProcessHello()
	: m_bIsSearching(FALSE)
	, m_lnSearchInterval(1000)
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

void CRoutingProcessHello::OnEngineTimer(int nCommandId)
{
	ContinueSearching();
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
		SendHelloPackage();
		EngineRegisterTimer(HELLO_TIMER_CMD_SEARCH, this, m_lnSearchInterval);
	}
}

void CRoutingProcessHello::ContinueSearching()
{
	if (m_bIsSearching)
	{
		SendHelloPackage();
		EngineRegisterTimer(HELLO_TIMER_CMD_SEARCH, this, m_lnSearchInterval);
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
