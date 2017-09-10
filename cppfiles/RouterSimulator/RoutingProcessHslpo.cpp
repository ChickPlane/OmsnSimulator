#include "stdafx.h"
#include "RoutingProcessHslpo.h"
#include "RoutingMsgHslpo.h"
#include "MobileSocialNetworkHost.h"


CRoutingProcessHslpo::CRoutingProcessHslpo()
	: m_nK(0)
	, m_fPrivacyHigh(100)
	, m_fPrivacyLow(100)
{
}


CRoutingProcessHslpo::~CRoutingProcessHslpo()
{
}

void CRoutingProcessHslpo::GenerateMission(const CRoutingDataEnc & encData)
{
	CRoutingMsgHslpo * pMsgBsw = new CRoutingMsgHslpo();
	pMsgBsw->InitByData(encData);
	pMsgBsw->InitValues_Data(m_pProtocol, 1);
	pMsgBsw->InitValues_AnonymityData(m_pProtocol, m_nK, m_fPrivacyHigh, m_fPrivacyLow);
	m_DataList.AddTail(pMsgBsw);
	StartBinarySprayWaitProcess();
}

void CRoutingProcessHslpo::SendData(CRoutingProtocol * pNext)
{
	POSITION pos = m_DataList.GetHeadPosition(), posLast;
	while (pos)
	{
		bool bSent = true;
		bool bDeleteMsg = false;
		posLast = pos;
		CRoutingMsgHslpo * pMsg = (CRoutingMsgHslpo*)m_DataList.GetNext(pos);

		if (pMsg->InObfuscation())
		{
			if (pMsg->IsParticipatedFriend(pNext))
			{
				bSent = false;
			}
			else
			{
				CMobileSocialNetworkHost * pSocialHost = (CMobileSocialNetworkHost *)m_pHost;
				CTrustValue fTrustValue;
				bool bIsFriend = pSocialHost->FindTrust(pNext->GetHostId(), fTrustValue);
				if (!bIsFriend)
				{
					bSent = false;
				}
				else
				{
					if (fTrustValue < pMsg->m_fPrivacyHigh)
					{
						if (fTrustValue < pMsg->m_fPrivacyLow)
						{
							bSent = false;
						}
						else
						{
							SIM_TIME lnTime = GetSimTime();
							CDoublePoint selfPosition = m_pProtocol->GetHostPostion(lnTime);
							CDoublePoint friendPosition = pNext->GetHostPostion(lnTime);
							CDoublePoint destPostion = pMsg->m_pDestination->GetHostPostion(lnTime);
							double fDisSelf = CDoublePoint::GetDistance(selfPosition, destPostion);
							double fDisFriend = CDoublePoint::GetDistance(friendPosition, destPostion);
							if (fDisSelf < fDisFriend)
							{
								bSent = false;
							}
						}
					}
				}
			}
			if (bSent == true)
			{
				bDeleteMsg = true;
			}
		}
		else
		{
			if (pNext == pMsg->m_pDestination)
			{
				bDeleteMsg = true;
			}
			else
			{
				if (pMsg->m_nCopyCount == 1)
				{
					bSent = false;
				}
				if (pMsg->IsKnownNode(pNext))
				{
					bSent = false;
				}
			}
		}

		if (bSent)
		{
			if (!pMsg->InObfuscation())
			{
				if (pMsg->m_KnownNodes.GetSize() == 1)
				{
					pMsg->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_BSW_BEGIN, HOP_INFO_TYPE_EVENT));
					m_pProtocol->EngineRecordPackage(pMsg->m_pData->GetDataId(m_pHost), pMsg->m_pData->m_Statistic, MSG_HOP_STATE_BSW_BEGIN);
				}
				pMsg->UpdateRecordMsg(pNext);
			}
			CRoutingMsgHslpo * pNew = new CRoutingMsgHslpo(*pMsg);
			pNew->SetSendValues_Data(m_pProtocol, pNext);
			if (pNew->InObfuscation())
			{
				if (pNew->IsFirstAnonymityHop())
				{
					pNew->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_BEGIN, HOP_INFO_TYPE_EVENT));
					m_pProtocol->EngineRecordPackage(pNew->m_pData->GetDataId(m_pHost), pNew->m_pData->m_Statistic, MSG_HOP_STATE_ANONYMITY_BEGIN);
				}
				pNew->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_TRANS, HOP_INFO_TYPE_EVENT));
				m_pProtocol->EngineRecordPackage(pNew->m_pData->GetDataId(m_pHost), pNew->m_pData->m_Statistic, MSG_HOP_STATE_ANONYMITY_TRANS);
				pNew->SetSendValues_AnonymityData(m_pProtocol);
			}
			TransmitMessage(pNext, pNew);
		}

		if (bDeleteMsg)
		{
			delete pMsg;
			m_DataList.RemoveAt(posLast);
		}
	}
}

void CRoutingProcessHslpo::OnReceivedData(const CRoutingMsg * pMsg)
{
	CRoutingMsgHslpo * pNewMsg = new CRoutingMsgHslpo(*(const CRoutingMsgHslpo*)pMsg);
	pNewMsg->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_OTHERS, HOP_INFO_TYPE_ARRIVE));
	if (pNewMsg->m_pDestination == m_pProtocol)
	{
		m_pProtocol->OnDelivered(pNewMsg);
		delete pNewMsg;
	}
	else
	{
		CRoutingMsgHslpo * pMsgRecord = (CRoutingMsgHslpo*)CheckDuplicateInDataList(pNewMsg->m_nBswId);
		if (pMsgRecord)
		{
			pMsgRecord->MergeMessage(*pNewMsg);
			delete pNewMsg;
		}
		else
		{
			if (pNewMsg->InObfuscation())
			{
				pNewMsg->m_nAnonymityCount--;
				if (!pNewMsg->InObfuscation())
				{
					pNewMsg->m_pData->m_Statistic.RecordInfo(m_pProtocol->GetMsgHopInfo(MSG_HOP_STATE_ANONYMITY_END, HOP_INFO_TYPE_EVENT));
					m_pProtocol->EngineRecordPackage(pNewMsg->m_pData->GetDataId(m_pHost), pNewMsg->m_pData->m_Statistic, MSG_HOP_STATE_ANONYMITY_END);
					pNewMsg->SetCopyCount(m_nCopyCount);
				}
			}
			m_DataList.AddTail(pNewMsg);
			StartBinarySprayWaitProcess();
		}
	}
}

void CRoutingProcessHslpo::SetPrivacyParam(int nK, double fHigh, double fLow)
{
	m_nK = nK;
	m_fPrivacyHigh = fHigh;
	m_fPrivacyLow = fLow;
}

int CRoutingProcessHslpo::GetAnonymityPackageCount() const
{
	int nRet = 0;
	POSITION pos = m_DataList.GetHeadPosition();
	while (pos)
	{
		CRoutingMsgHslpo * pMsg = (CRoutingMsgHslpo*)m_DataList.GetNext(pos);
		if (pMsg->InObfuscation())
		{
			++nRet;
		}
	}
	return nRet;
}
