#pragma once
#include "RoutingMsgBSW.h"
#include "TrustValue.h"
#include "RoutingDataEnc.h"

class CRoutingProtocolHslpo;

typedef enum
{
	HSLPO_MSG_TYPE_DATA = BSW_MSG_TYPE_MAX,
}HSLPO_MSG_TYPE;

class CRoutingMsgHslpo :
	public CRoutingMsgBSW
{
public:
	CRoutingMsgHslpo();
	CRoutingMsgHslpo(const CRoutingMsgHslpo & src);
	CRoutingMsgHslpo & operator = (const CRoutingMsgHslpo & src);
	~CRoutingMsgHslpo();

	void InitValues_AnonymityData(CRoutingProtocol * pRequester, int nK, double fHigh, double fLow);
	void SetSendValues_AnonymityData(CRoutingProtocol * pFrom);
	bool IsParticipatedFriend(CRoutingProtocol * pTest);
	bool InObfuscation() const;
	bool IsFirstAnonymityHop() const;

	int m_nAnonymityCount;
	CTrustValue m_fPrivacyHigh;
	CTrustValue m_fPrivacyLow;
	CList<CRoutingProtocol *> m_ParticipatedFriends;
	CRoutingProtocol * m_pLastAnonymityFriend;
};

