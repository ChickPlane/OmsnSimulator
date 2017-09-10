#pragma once
#include "ProtocolInfo.h"
class CHostInfo
{
public:
	CHostInfo();
	~CHostInfo();

	void Reset();

	int m_nHostId;
	CProtocolInfo m_protocolInfo;
};

