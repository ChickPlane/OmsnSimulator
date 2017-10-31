#include "stdafx.h"
#include "RoutingProcessSlpd.h"
#include "SentenceSlpd.h"


CRoutingProcessSlpd::CRoutingProcessSlpd()
{
}


CRoutingProcessSlpd::~CRoutingProcessSlpd()
{
}

void CRoutingProcessSlpd::SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol)
{
	CRoutingProcess::SetBasicParameters(nProcessID, pProtocol);
}

void CRoutingProcessSlpd::SetParameters(int nK)
{
	m_nK = nK;
}

void CRoutingProcessSlpd::OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList)
{
	int nSentenceType = pPkg->m_nSentenceType;
	switch (nSentenceType)
	{
	case SLPD_SENTENCE_TYPE_DATA:
	{
		OnReceiveNewPseudoPkg((const CPkgSlpd*)pPkg);
		break;
	}
	default:
	{
		ASSERT(0);
	}
	}
}

void CRoutingProcessSlpd::OnEncounterUser(CRoutingProtocol * pTheOther, CList<CSentence *> & SendingList)
{
	if (!m_pUser->IsTrustful(this, pTheOther))
	{
		return;
	}
	if (m_ForwardingList.GetSize() == 0)
	{
		return;
	}
	CTimeOutPair<CPkgSlpd *>::DeleteTimePairs(m_ForwardingList, GetSimTime());
	POSITION pos = m_ForwardingList.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CPkgSlpd * pPkg = m_ForwardingList.GetNext(pos).m_Value;
		if (pPkg->IsAlreadyInFriendList(pTheOther->GetHostId()))
		{
			continue;
		}
		if (pPkg->m_nRemainTimes == m_nK)
		{
			m_pUser->OnFirstSlpdObfuscationForward(this, pPkg);
		}
		MarkProcessIdToSentences(pPkg);
		pPkg->m_pSpeakTo = pTheOther;
		SendingList.AddTail(pPkg);
		m_ForwardingList.RemoveAt(posLast);
	}
}

void CRoutingProcessSlpd::CreateQueryMission(CPkgSlpd * pPkg)
{
	pPkg->m_FriendList.RemoveAll();
	pPkg->m_FriendList.AddHead(m_pProtocol->GetHostId());
	pPkg->m_lnTimeout = pPkg->m_pTestSession->m_lnTimeOut;
	pPkg->m_nPseudonym = ++gm_PseudonymMax;
	pPkg->m_nRemainTimes = m_nK+1;
	OnReceiveNewPseudoPkg(pPkg);
}

BOOL CRoutingProcessSlpd::IsInPseudonymList(SLPD_USERID lnPseudonym, BOOL bAutoDelete, CSlpdUserAndPseudo & ret)
{
	POSITION pos = m_PseudonymList.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CTimeOutPair<CSlpdUserAndPseudo> & tmpUP = m_PseudonymList.GetNext(pos);
		if (tmpUP.m_Value.m_lnPseudonym == lnPseudonym)
		{
			ret = tmpUP.m_Value;
			if (bAutoDelete)
			{
				m_PseudonymList.RemoveAt(posLast);
			}
			return TRUE;
		}
	}
	return FALSE;
}

int CRoutingProcessSlpd::GetObfuscationCount() const
{
	return m_ForwardingList.GetSize();
}

void CRoutingProcessSlpd::OnReceiveNewPseudoPkg(const CPkgSlpd * pPkg)
{
	SIM_TIME lnCT = GetSimTime();
	if (pPkg->m_lnTimeout < lnCT)
	{
		return;
	}

	CString strLog;
	strLog.Format(_T("[slpd] %d %d %d"), pPkg->m_pTestSession->m_nSessionId, m_pProtocol->GetHostId(), pPkg->m_nRemainTimes);
	WriteLog(strLog);

	CTimeOutPair<CSlpdUserAndPseudo> pairUserId;
	pairUserId.m_lnTimeOut = pPkg->m_lnTimeout;
	pairUserId.m_Value.m_lnPseudonym = pPkg->m_nPseudonym;
	pairUserId.m_Value.m_lnUserId = pPkg->GetOriginalRequester();
	CTimeOutPair<CSlpdUserAndPseudo>::InsertToTimeoutPairList(pairUserId, m_PseudonymList);

	if (pPkg->m_nRemainTimes == 0)
	{
		m_pUser->OnNewSlpdPseudoOver(this, pPkg);
	}
	else if(pPkg->m_nRemainTimes > 0)
	{
		m_pUser->OnNewSlpdPseudo(this, pPkg);

		CPkgSlpd * pNewPkg = m_pUser->GetSlpdDataCopy(this, pPkg);
		--pNewPkg->m_nRemainTimes;

		CTimeOutPair<CPkgSlpd *> pairPkg;
		pairPkg.m_lnTimeOut = pNewPkg->m_lnTimeout;
		pairPkg.m_Value = pNewPkg;
		CTimeOutPair<CPkgSlpd *>::InsertToTimeoutPairList(pairPkg, m_ForwardingList);
	}
}

int CRoutingProcessSlpd::gm_PseudonymMax = 0;

int CRoutingProcessSlpd::m_nK = 1;

CPkgSlpd * CRoutingProcessSlpdUser::GetSlpdDataCopy(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg)
{
	return new CPkgSlpd(*pPkg);
}
