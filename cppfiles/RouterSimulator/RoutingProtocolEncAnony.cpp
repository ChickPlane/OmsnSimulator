#include "stdafx.h"
#include "RoutingProtocolEncAnony.h"
#include "RoutingMsgEncAnony.h"
#include "HostEngine.h"
#include "MobileSocialNetworkHost.h"
#include "RoutingProcessBSW.h"


#define ENC_ANONY_PMHNS 0
#define ENC_ANONY_BSW 1

CRoutingProtocolEncAnony::CRoutingProtocolEncAnony()
	: m_bInSearching(false)
{
	m_pPmhns = new CRoutingProcessPmhns();
	m_nPmhnsId = AddProcess(m_pPmhns);
	m_pPmhns->m_pUser = this;
	m_pBSW = new CRoutingProcessBSW();
	m_nBswId = AddProcess(m_pBSW);
}


CRoutingProtocolEncAnony::~CRoutingProtocolEncAnony()
{
}

void CRoutingProtocolEncAnony::SetPrivacyParam(double fTrustBound, double fRp, double fRs, double fCostMax)
{
	m_fTrustBound = fTrustBound;
	m_fRp = fRp;
	m_fRs = fRs;
	m_fCostMax = fCostMax;
}

void CRoutingProtocolEncAnony::SetCopyCount(int nCopyCount)
{
	CRoutingProcessBSW * pBswProcess = (CRoutingProcessBSW*)m_Processes[m_nBswId];
	pBswProcess->SetCopyCount(nCopyCount);
}

void CRoutingProtocolEncAnony::OnReceivedMsg(const CRoutingMsg * pMsg)
{
	if (!pMsg->IsBelongTo(this))
	{
		return;
	}
	switch (pMsg->m_nProtocolType)
	{
	case ENC_ANONY_PMHNS:
	{
		m_pPmhns->OnReceivedMsg(pMsg);
		break;
	}
	case ENC_ANONY_BSW:
	{
		m_pBSW->OnReceivedMsg(pMsg);
		break;
	}
	default:
	{
		break;
	}
	}
}

void CRoutingProtocolEncAnony::SendPackage(const CRoutingDataEnc & encData)
{
	ASSERT(encData.CanRead(GetHost()));
	CEncAnonyPacket * pNewEncAnonyMsg = new CEncAnonyPacket();
	pNewEncAnonyMsg->CRoutingDataEnc::operator =(encData);
	pNewEncAnonyMsg->fRp = m_fRp;
	pNewEncAnonyMsg->fRs = m_fRs;
	pNewEncAnonyMsg->fMaxCost = m_fCostMax;
	pNewEncAnonyMsg->fTrustBound = m_fTrustBound;
	pNewEncAnonyMsg->fUsedCost = 0;

	RecordNewPackage(*pNewEncAnonyMsg);
	m_AnonymityList.AddTail(pNewEncAnonyMsg);
	StartFriendSearch();
}

void CRoutingProtocolEncAnony::OnEngineTimer(int nCommandId)
{
	switch (nCommandId)
	{
	case EA_TIMER_CMD_FRIEND_SEARCH:
	{
		CheckFriendSearch();
		break;
	}
	}
}

void CRoutingProtocolEncAnony::CleanTimeOutData()
{
	POSITION pos = m_AnonymityList.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CEncAnonyPacket * pPacket = m_AnonymityList.GetNext(pos);
		SIM_TIME lnDataTimeOut = pPacket->GetTimeOut(GetHost());
		if (lnDataTimeOut < GetSimTime())
		{
			delete pPacket;
			m_AnonymityList.RemoveAt(posLast);
		}
	}
}

void CRoutingProtocolEncAnony::StartFriendSearch()
{
	if (m_bInSearching == false)
	{
		m_bInSearching = true;
		m_pPmhns->GetRoutings();
		EngineRegisterTimer(EA_TIMER_CMD_FRIEND_SEARCH, this, 1000);
	}
}

void CRoutingProtocolEncAnony::CheckFriendSearch()
{
	CleanTimeOutData();
	CheckPosition();
	if (m_AnonymityList.GetSize() > 0)
	{
		m_pPmhns->GetRoutings();
		EngineRegisterTimer(EA_TIMER_CMD_FRIEND_SEARCH, this, 1000);
		ASSERT(m_bInSearching == true);
	}
	else
	{
		m_bInSearching = false;
	}
}

void CRoutingProtocolEncAnony::CheckPosition()
{
	SIM_TIME lnTime = GetSimTime();
	CDoublePoint currentlocation = GetHost()->GetPosition(lnTime);
	POSITION pos = m_AnonymityList.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CEncAnonyPacket * pPacket = m_AnonymityList.GetNext(pos);
		double fDistance = CDoublePoint::GetDistance(pPacket->lastPosition, currentlocation);
		if (fDistance >= pPacket->fRp && fDistance <= pPacket->fRs)
		{
			pPacket->m_Statistic.RecordInfo(GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_END, HOP_INFO_TYPE_EVENT));
			EngineRecordPackage(pPacket->GetDataId(GetHost()), pPacket->m_Statistic, MSG_HOP_STATE_ANONYMITY_END);
			CRoutingMsg freePackage;
			freePackage.InitByData(*pPacket);
			freePackage.m_pSource = this;
			SendFreeData(freePackage);
			delete pPacket;
			m_AnonymityList.RemoveAt(posLast);
		}
	}
}

void CRoutingProtocolEncAnony::OnGetPmhnsRep(const CList<CRoutingProtocol*> & path)
{
	CRoutingProtocol * pTail = path.GetTail();
	int nTailId = pTail->GetHost()->m_nId;
	CMobileSocialNetworkHost * pHost = (CMobileSocialNetworkHost*)GetHost();
	CTrustValue tv;
	if (pHost->FindTrust(nTailId, tv))
	{
		SendAnonymityData(tv, path);
	}
}

void CRoutingProtocolEncAnony::OnGetPmhnsData(const CRoutingMsg * pMsg)
{
	CEncAnonyPacket * pNewEncAnonyMsg = new CEncAnonyPacket(*(CEncAnonyPacket*)pMsg->m_pData);
	double fDistance = CDoublePoint::GetDistance(pNewEncAnonyMsg->lastPosition, GetHost()->GetPosition(GetSimTime()));
	if (fDistance >= pNewEncAnonyMsg->fRp && fDistance <= pNewEncAnonyMsg->fRs)
	{
		pNewEncAnonyMsg->m_Statistic.RecordInfo(GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_END, HOP_INFO_TYPE_EVENT));
		EngineRecordPackage(pNewEncAnonyMsg->GetDataId(GetHost()), pNewEncAnonyMsg->m_Statistic, MSG_HOP_STATE_ANONYMITY_END);

		CRoutingMsg freePackage;
		freePackage.InitByData(*pNewEncAnonyMsg);
		freePackage.m_pSource = this;
		SendFreeData(freePackage);
	}
	else
	{
		m_AnonymityList.AddTail(pNewEncAnonyMsg);
		StartFriendSearch();
	}
}

void CRoutingProtocolEncAnony::SendAnonymityData(const CTrustValue & tv, const CList<CRoutingProtocol*> & path)
{
	double fTrustValue = (double)tv;
	CHost * pNextHost = path.GetTail()->GetHost();
	SIM_TIME simTime = GetSimTime();
	POSITION pos = m_AnonymityList.GetHeadPosition(), posLast;
	CHost * pSelfHost = GetHost();
	CDoublePoint selfPosition = pSelfHost->GetPosition(simTime);
	while (pos)
	{
		posLast = pos;
		CEncAnonyPacket * pData = m_AnonymityList.GetNext(pos);
		if (pData->fTrustBound <= fTrustValue || path.GetTail() == pData->pLastHop)
		{
			if (!pData->bHolding)
			{
				double fSelfDistance = pData->GetDistance(selfPosition);
				double fNextDistance = pData->GetDistance(pNextHost->GetPosition(simTime));
				if (fNextDistance < 0)
				{
					continue;
				}
				if (fNextDistance > fSelfDistance)
				{
					continue;
				}
			}
			else
			{
				pData->bHolding = false;
				pData->lastPosition = selfPosition;
				pData->m_Statistic.RecordInfo(GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_BEGIN, HOP_INFO_TYPE_EVENT));
				EngineRecordPackage(pData->GetDataId(pSelfHost), pData->m_Statistic, MSG_HOP_STATE_ANONYMITY_BEGIN);
			}

			pData->m_Statistic.RecordInfo(GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_TRANS, HOP_INFO_TYPE_EVENT));
			EngineRecordPackage(pData->GetDataId(pSelfHost), pData->m_Statistic, MSG_HOP_STATE_ANONYMITY_TRANS);

			m_pPmhns->SendDataPackage(path, *pData);
			delete pData;
			pData = NULL;
			m_AnonymityList.RemoveAt(posLast);
		}
	}
}

void CRoutingProtocolEncAnony::SendFreeData(const CRoutingMsg & Msg)
{
	m_pBSW->GenerateMission(*Msg.m_pData);
}

int CRoutingProtocolEncAnony::GetInfoList(CList<CString> & ret)
{
	SIM_TIME lnTime = GetSimTime();
	CDoublePoint currentlocation = GetHost()->GetPosition(lnTime);
	POSITION pos = m_AnonymityList.GetHeadPosition(), posLast;
	while (pos)
	{
		CEncAnonyPacket * pPacket = m_AnonymityList.GetNext(pos);
		double fDistance = CDoublePoint::GetDistance(pPacket->lastPosition, currentlocation);
		CString strItem;
		int nDataId = pPacket->GetTimeOut(GetHost());
		if (pPacket->bHolding)
		{
			strItem.Format(_T("[%6d] distance(0)"), nDataId);
		}
		else
		{
			strItem.Format(_T("[%6d] distance(%f)"), nDataId, (float)fDistance);
		}
		ret.AddTail(strItem);
	}
	return ret.GetSize();
}

void CRoutingProtocolEncAnony::GetAllCarryingMessages(CList<CRoutingDataEnc> & ret) const
{
	CRoutingProtocol::GetAllCarryingMessages(ret);
	
	POSITION pos = m_AnonymityList.GetHeadPosition();
	while (pos)
	{
		CEncAnonyPacket * pPacket = m_AnonymityList.GetNext(pos);
		ret.AddTail(*pPacket);
	}
}

int CRoutingProtocolEncAnony::GetInportantLevel() const
{
	if (m_AnonymityList.GetSize() > 0)
	{
		return 2;
	}
	if (m_pBSW->m_DataList.GetSize() > 0)
	{
		return 1;
	}
	return 0;
}

CTrustValue CRoutingProtocolEncAnony::TrustFunction(double fDistance)
{
	return fDistance;
}

CEncAnonyPacket::CEncAnonyPacket()
	: fRp(0)
	, fRs(0)
	, fTrustBound(0)
	, fMaxCost(0)
	, fUsedCost(0)
	, bHolding(true)
{
}

CEncAnonyPacket::CEncAnonyPacket(const CEncAnonyPacket & src)
{
	*this = src;
}

CEncAnonyPacket * CEncAnonyPacket::GetDuplicate() const
{
	CEncAnonyPacket * pRet = new CEncAnonyPacket(*this);
	return pRet;
}

double CEncAnonyPacket::GetDistance(const CDoublePoint & test)
{
	double fDistance = CDoublePoint::GetDistance(test, lastPosition);
	if (fDistance >= fRp && fDistance <= fRs)
	{
		return 0;
	}
	else
	{
		if (fDistance < fRp)
		{
			return fRp - fDistance;
		}
		if (fDistance > fRs)
		{
			return fRs - fDistance;
		}
	}
}

void CEncAnonyPacket::ChangeValue(double uc)
{
	fUsedCost = uc;
}

CEncAnonyPacket & CEncAnonyPacket::operator=(const CEncAnonyPacket & src)
{
	CRoutingDataEnc::operator=(src);
	lastPosition = src.lastPosition;
	fRp = src.fRp;
	fRs = src.fRs;
	fTrustBound = src.fTrustBound;
	fMaxCost = src.fMaxCost;
	fUsedCost = src.fUsedCost;
	bHolding = src.bHolding;
	pLastHop = src.pLastHop;
	return *this;
}
