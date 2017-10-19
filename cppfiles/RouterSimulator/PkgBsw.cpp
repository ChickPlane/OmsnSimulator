#include "stdafx.h"
#include "PkgBsw.h"
#include "SimulatorCommon.h"
#include "TestSession.h"


CPkgBswData::CPkgBswData()
	: m_nCopyCount(0)
	, m_uReceiverId(UID_LBSP0)
	, m_pTestSession(NULL)
{
	ChangeId();
}


CPkgBswData::CPkgBswData(const CPkgBswData & src)
{
	*this = src;
}

CPkgBswData & CPkgBswData::operator=(const CPkgBswData & src)
{
	CSentence::operator=(src);
	m_KnownNodes.RemoveAll();
	POSITION pos = src.m_KnownNodes.GetHeadPosition();
	while (pos)
	{
		m_KnownNodes.AddTail(src.m_KnownNodes.GetNext(pos));
	}
	m_nCopyCount = src.m_nCopyCount;
	m_nBswId = src.m_nBswId;
	m_uReceiverId = src.m_uReceiverId;
	m_pTestSession = src.DeepCopySession();
	m_bLastHop = src.m_bLastHop;
	return *this;
}

CPkgBswData::~CPkgBswData()
{
}

BOOL CPkgBswData::IsReceiver(BSW_USERID uId) const
{
	return uId == m_uReceiverId;
}

BOOL CPkgBswData::IsKnownNode(CRoutingProtocol * pTest) const
{
	POSITION posSelf = m_KnownNodes.GetHeadPosition();
	while (posSelf)
	{
		CRoutingProtocol * pSelf = (CRoutingProtocol*)m_KnownNodes.GetNext(posSelf);
		if (pTest == pSelf)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CTestSession * CPkgBswData::DeepCopySession() const
{
	if (m_pTestSession)
	{
		return new CTestSession(*m_pTestSession);
	}
	else
	{
		return NULL;
	}
}

void CPkgBswData::HalfCount(BOOL bBottom)
{
	m_nCopyCount = GetHalfCopyCount(bBottom);
}

int CPkgBswData::GetHalfCopyCount(BOOL bBottom) const
{
	if (bBottom)
	{
		return (m_nCopyCount) / 2;
	}
	else
	{
		return (m_nCopyCount + 1) / 2;
	}
}

void CPkgBswData::ChangeId()
{
	m_nBswId = ++sm_nBswIdMax;
}


void CPkgBswData::MergeMessage(const CPkgBswData & src)
{
	POSITION posOther = src.m_KnownNodes.GetHeadPosition();
	while (posOther)
	{
		CRoutingProtocol * pTest = (CRoutingProtocol*)src.m_KnownNodes.GetNext(posOther);
		bool bFound = IsKnownNode(pTest);
		if (!bFound)
		{
			m_KnownNodes.AddTail(pTest);
		}
	}
	m_nCopyCount += src.m_nCopyCount;
}

void CPkgBswData::InitParameters(int nCopyCount, BSW_USERID uReceiverId)
{
	m_nCopyCount = nCopyCount;
	m_uReceiverId = uReceiverId;
	m_bLastHop = FALSE;
}

BOOL CPkgBswData::OnlyOneCopyLeft()
{
	return (m_nCopyCount == 1);
}

BSW_USERID CPkgBswData::GetReceiverId() const
{
	return m_uReceiverId;
}

int CPkgBswData::sm_nBswIdMax = 0;
