#include "stdafx.h"
#include "EngineEvent.h"


CEngineEvent::CEngineEvent()
	: m_bForecastStack(FALSE)
{
}


CEngineEvent::CEngineEvent(const CEngineEvent & src)
{
	*this = src;
}

CEngineEvent & CEngineEvent::operator=(const CEngineEvent & src)
{
	m_bForecastStack = src.m_bForecastStack;
	m_lnSimTime = src.m_lnSimTime;
	m_nCommandId = src.m_nCommandId;
	m_pUser = src.m_pUser;
	return *this;
}

CEngineEvent::~CEngineEvent()
{
}
