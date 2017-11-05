#include "stdafx.h"
#include "RoutingProcessBsw.h"
#include "RoutingProtocol.h"


BOOL CRoutingProcessBswUser::IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger)
{
	return pPkg->m_uReceiverId == pTheOther->GetHostId();
}

CRoutingProcessBsw::CRoutingProcessBsw()
	: m_pUser(NULL)
	, m_nCopyCount(1)
{
}


CRoutingProcessBsw::~CRoutingProcessBsw()
{
}

void CRoutingProcessBsw::SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol)
{
	CRoutingProcess::SetBasicParameters(nProcessID, pProtocol);
}

void CRoutingProcessBsw::InitNewPackage(CPkgBswData * pPkg, BSW_USERID uReceiverId, SIM_TIME lnTimeOut)
{
	pPkg->m_uReceiverId = uReceiverId;
	pPkg->m_lnTimeOut = lnTimeOut;
	InitNewPackage(pPkg);
}

void CRoutingProcessBsw::InitNewPackage(CPkgBswData * pPkg)
{
	pPkg->m_KnownNodes.RemoveAll();
	pPkg->m_KnownNodes.AddTail(m_pProtocol);
	pPkg->ChangeId();
	pPkg->m_nCopyCount = m_nCopyCount;
	pPkg->m_uSenderId = m_pProtocol->GetHostId();
	pPkg->m_bLastHop = FALSE;
}

void CRoutingProcessBsw::OnEncounterUser(CRoutingProtocol * pTheOther, CList<CSentence *> & SendingList, const CSentence * pTriger)
{
	CleanTimeoutData();
	CList<int> toBeRemoved;
	POSITION pos = m_DataMap.GetStartPosition();
	while (pos)
	{
		bool bSent = true;
		bool bDeleteMsg = false;
		int rKey;
		CPkgBswData * pPkg = NULL;
		m_DataMap.GetNextAssoc(pos, rKey, pPkg);
		if (pPkg->m_lnTimeOut < GetSimTime())
		{
			bSent = false;
			continue;
		}

		if (m_pUser->IsTheLastHop(this, pPkg, pTheOther, pTriger))
		{
			bDeleteMsg = true;
		}
		else
		{
			if (pPkg->OnlyOneCopyLeft())
			{
				bSent = false;
			}
			if (pPkg->IsKnownNode(pTheOther))
			{
				bSent = false;
			}
		}

		if (bSent)
		{
			if (pPkg->m_KnownNodes.GetSize() == 1)
			{
				m_pUser->OnPackageFirstSent(this, pPkg);
			}
			CPkgBswData * pNew = m_pUser->CopyPackage(this, pPkg);
			if (!bDeleteMsg)
			{
				pPkg->m_KnownNodes.AddTail(pTheOther);
				pPkg->HalfCount(FALSE);

				pNew->HalfCount(TRUE);
			}
			else
			{
				delete pPkg;
				toBeRemoved.AddTail(rKey);
				pNew->m_bLastHop = TRUE;
			}
			pNew->m_pSpeakTo = pTheOther;
			MarkProcessIdToSentences(pNew);
			SendingList.AddTail(pNew);
		}
	}

	pos = toBeRemoved.GetHeadPosition();
	while (pos)
	{
		m_DataMap.RemoveKey(toBeRemoved.GetNext(pos));
	}
}

void CRoutingProcessBsw::OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList)
{
	int nSentenceType = pPkg->m_nSentenceType;
	ASSERT(nSentenceType == BSW_MSG_TYPE_DATA);

	CPkgBswData * pBswPkg = (CPkgBswData *)pPkg;
	if (pBswPkg->m_bLastHop)
	{
		if (m_pUser->IsPackageForMe(this, pBswPkg))
		{
			SIM_TIME lnRecordTimeout = 0;
			if (pBswPkg->m_lnTimeOut < GetSimTime())
			{
				return;
			}
			if (IsDestRecvRecordExist(pBswPkg->m_nBswId))
			{
				return;
			}
			CBswIdAndTimeout bt;
			bt.m_nBswId = pBswPkg->m_nBswId;
			bt.m_lnTimeout = pBswPkg->m_lnTimeOut;
			m_pUser->OnBswPkgReachDestination(this, pBswPkg);
			InsertToDestinationRecvRecord(bt);
			int k = 3;
			++k;
		}
		else
		{
			OutputDebugString(_T("\nBSW RECEIVE WRONG MSG."));
		}
	}
	else
	{
		CPkgBswData * pMsgRecord = CheckDuplicateInMap(pBswPkg->m_nBswId);
		if (pMsgRecord)
		{
			pMsgRecord->MergeMessage(*pBswPkg);
		}
		else
		{
			CPkgBswData * pNewRelayReply = m_pUser->CopyPackage(this, pBswPkg);
			pNewRelayReply->m_KnownNodes.AddTail(m_pProtocol);
			InsertToDataMap(pNewRelayReply);
		}
	}
}

void CRoutingProcessBsw::SetCopyCount(int nCopyCount)
{
	m_nCopyCount = nCopyCount;
}

int CRoutingProcessBsw::GetDataMapSize() const
{
	return m_DataMap.GetSize();
}

void CRoutingProcessBsw::CleanTimeoutData()
{
	SIM_TIME lnTime = GetSimTime();
	
	POSITION pos = m_DataTimeoutList.GetHeadPosition(), prevpos;
	while (pos)
	{
		prevpos = pos;
		const CBswIdAndTimeout& tmpDTL = m_DataTimeoutList.GetNext(pos);
		if (tmpDTL.m_lnTimeout < lnTime)
		{
			CPkgBswData * pValue = NULL;
			if (m_DataMap.Lookup(tmpDTL.m_nBswId, pValue))
			{
				delete pValue;
				m_DataMap.RemoveKey(tmpDTL.m_nBswId);
			}
			m_DataTimeoutList.RemoveAt(prevpos);
		}
		else
		{
			break;
		}
	}
}

void CRoutingProcessBsw::CleanTimeoutPkgRecord()
{
	SIM_TIME lnTime = GetSimTime();

	POSITION pos = m_PkgRecvRecord.GetHeadPosition(), prevpos;
	while (pos)
	{
		prevpos = pos;
		const CBswIdAndTimeout& tmpDTL = m_PkgRecvRecord.GetNext(pos);
		if (tmpDTL.m_lnTimeout < lnTime)
		{
			m_PkgRecvRecord.RemoveAt(prevpos);
		}
		else
		{
			break;
		}
	}
}

CPkgBswData * CRoutingProcessBsw::CheckDuplicateInMap(int nBswId)
{
	CPkgBswData * rValue;
	if (m_DataMap.Lookup(nBswId, rValue))
	{
		return rValue;
	}
	return NULL;
}

void CRoutingProcessBsw::InsertToDataMap(CPkgBswData* pPkg)
{
	CPkgBswData * rValue;
	if (m_DataMap.Lookup(pPkg->m_nBswId, rValue))
	{
		ASSERT(0);
		return;
	}

	CBswIdAndTimeout BT;
	BT.m_lnTimeout = pPkg->m_lnTimeOut;
	BT.m_nBswId = pPkg->m_nBswId;
	InsertToBTList(BT, m_DataTimeoutList);

	m_DataMap[BT.m_nBswId] = pPkg;
}

void CRoutingProcessBsw::InsertToDestinationRecvRecord(const CBswIdAndTimeout & BT)
{
	InsertToBTList(BT, m_PkgRecvRecord);
	CString strOut;
	strOut.Format(_T("\n[BSW] %d insert %d"), m_pProtocol->GetHostId(), BT.m_nBswId);
	OutputDebugString(strOut);
}

BOOL CRoutingProcessBsw::IsDestRecvRecordExist(int nBswId)
{
	POSITION pos = m_PkgRecvRecord.GetHeadPosition();
	while (pos)
	{
		CBswIdAndTimeout& tmpDTL = m_PkgRecvRecord.GetNext(pos);
		if (tmpDTL.m_nBswId == nBswId)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CRoutingProcessBsw::InsertToBTList(const CBswIdAndTimeout & newItem, CList<CBswIdAndTimeout> & BTList)
{
	POSITION pos = BTList.GetHeadPosition(), prevpos;
	while (pos)
	{
		prevpos = pos;
		CBswIdAndTimeout& bt = BTList.GetNext(pos);
		if (bt.m_lnTimeout > newItem.m_lnTimeout)
		{
			BTList.InsertBefore(prevpos, newItem);
			return;
		}
	}
	BTList.AddTail(newItem);
}
