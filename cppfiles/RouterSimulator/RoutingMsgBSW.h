#pragma once
#include "RoutingMsg.h"
class CHost;
class CRoutingProtocolBSW;

typedef enum
{
	BSW_MSG_TYPE_DATA,
	BSW_MSG_TYPE_HELLO,
	BSW_MSG_TYPE_HELLO_ACK,
	BSW_MSG_TYPE_MAX,
}BSW_MSG_TYPE;

#define INVALID_BSW_ID 0


class CRoutingMsgBSW :
	public CRoutingMsg
{
public:
	CRoutingMsgBSW();
	CRoutingMsgBSW(const CRoutingMsgBSW & src);
	CRoutingMsgBSW & operator = (const CRoutingMsgBSW & src);
	~CRoutingMsgBSW();

	void InitValues_Data(CRoutingProtocol * pFrom, int nCopyCount);
	void InitValues_Hello(CRoutingProtocol * pFrom);
	void InitValues_HelloAck(CRoutingProtocol * pFrom, CRoutingProtocol * pTo);
	void SetSendValues_Data(CRoutingProtocol * pFrom, CRoutingProtocol * pTo);
	void UpdateRecordMsg(CRoutingProtocol * pKnownProtocol);
	void SetCopyCount(int nCopyCount) { m_nCopyCount = nCopyCount; }

	void MergeMessage(const CRoutingMsgBSW & src);
	bool IsKnownNode(CRoutingProtocol * pTest) const;

	CList<CRoutingProtocol *> m_KnownNodes;
	int m_nCopyCount;
	int m_nBswId;

private:
	static int sm_nMaxBswId;
	void ChangeBswId();
};
