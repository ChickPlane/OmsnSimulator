#pragma once
#include "RoutingMsgBSW.h"

class CRoutingProtocolEncAnony;

typedef enum
{
	EA_MSG_TYPE_DATA = BSW_MSG_TYPE_MAX,
}EA_MSG_TYPE;

class CRoutingMsgEncAnony :
	public CRoutingMsg
{
public:
	CRoutingMsgEncAnony();
	CRoutingMsgEncAnony(const CRoutingMsgEncAnony & src);
	CRoutingMsgEncAnony(const CRoutingMsg & src);
	CRoutingMsgEncAnony & operator = (const CRoutingMsgEncAnony & src);
	~CRoutingMsgEncAnony();

	void InitValues_AnonymityData(CRoutingProtocolEncAnony * pFrom, double fPrivacyParam);
	void SetSendValues_AnonymityData(CRoutingProtocolEncAnony * pFrom, CRoutingProtocolEncAnony * pTo);

	double m_fPrivacyParam;
};

