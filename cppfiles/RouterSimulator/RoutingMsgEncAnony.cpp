#include "stdafx.h"
#include "RoutingMsgEncAnony.h"
#include "RoutingProtocolEncAnony.h"
#include "Host.h"


CRoutingMsgEncAnony::CRoutingMsgEncAnony()
{
}


CRoutingMsgEncAnony::CRoutingMsgEncAnony(const CRoutingMsgEncAnony & src)
{

}

CRoutingMsgEncAnony::CRoutingMsgEncAnony(const CRoutingMsg & src)
{
	*this = src;
}

CRoutingMsgEncAnony & CRoutingMsgEncAnony::operator=(const CRoutingMsgEncAnony & src)
{
	m_fPrivacyParam = src.m_fPrivacyParam;
	return *this;
}

CRoutingMsgEncAnony::~CRoutingMsgEncAnony()
{
}

void CRoutingMsgEncAnony::InitValues_AnonymityData(CRoutingProtocolEncAnony * pFrom, double fPrivacyParam)
{
	m_nMsgType = EA_MSG_TYPE_DATA;
	m_pFrom = pFrom;
	m_pTo = NULL;
	m_fPrivacyParam = fPrivacyParam;
}

void CRoutingMsgEncAnony::SetSendValues_AnonymityData(CRoutingProtocolEncAnony * pFrom, CRoutingProtocolEncAnony * pTo)
{
	m_nMsgType = EA_MSG_TYPE_DATA;
	m_pFrom = pFrom;
	m_pTo = pTo;
}
