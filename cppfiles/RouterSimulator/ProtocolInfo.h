#pragma once
#include "MsgInfo.h"
class CProtocolInfo
{
public:
	CProtocolInfo();
	~CProtocolInfo();

	void Reset();

	CString m_strSummary;
	CList<CMsgInfo> m_MsgInfos;
};

