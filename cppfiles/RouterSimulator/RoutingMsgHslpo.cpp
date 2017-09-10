#include "stdafx.h"
#include "RoutingMsgHslpo.h"
#include "RoutingProtocolBSW.h"
#include "RoutingProtocolHslpo.h"
#include "RoutingProtocol.h"
#include "Host.h"


CRoutingMsgHslpo::CRoutingMsgHslpo()
	: m_nAnonymityCount(0)
	, m_pLastAnonymityFriend(NULL)
{
}


CRoutingMsgHslpo::CRoutingMsgHslpo(const CRoutingMsgHslpo & src)
{
	*this = src;
}

CRoutingMsgHslpo & CRoutingMsgHslpo::operator=(const CRoutingMsgHslpo & src)
{
	CRoutingMsgBSW::operator=(src);
	m_nAnonymityCount = src.m_nAnonymityCount;
	m_fPrivacyHigh = src.m_fPrivacyHigh;
	m_fPrivacyLow = src.m_fPrivacyLow;
	m_pLastAnonymityFriend = src.m_pLastAnonymityFriend;
	return *this;
}

CRoutingMsgHslpo::~CRoutingMsgHslpo()
{
}

void CRoutingMsgHslpo::InitValues_AnonymityData(CRoutingProtocol * pRequester, int nK, double fHigh, double fLow)
{
	m_nAnonymityCount = nK;
	m_fPrivacyHigh = fHigh;
	m_fPrivacyLow = fLow;
	m_pLastAnonymityFriend = NULL;
}

void CRoutingMsgHslpo::SetSendValues_AnonymityData(CRoutingProtocol * pFrom)
{
	m_pLastAnonymityFriend = pFrom;
}

bool CRoutingMsgHslpo::IsParticipatedFriend(CRoutingProtocol * pTest)
{
	return (m_pLastAnonymityFriend == pTest);
}

bool CRoutingMsgHslpo::InObfuscation() const
{
	return (m_nAnonymityCount > 0);
}

bool CRoutingMsgHslpo::IsFirstAnonymityHop() const
{
	return (m_pLastAnonymityFriend == NULL);
}
