#include "stdafx.h"
#include "ProtocolInfo.h"


CProtocolInfo::CProtocolInfo()
{
}


CProtocolInfo::~CProtocolInfo()
{
}

void CProtocolInfo::Reset()
{
	m_strSummary.Empty();
	m_MsgInfos.RemoveAll();
}
