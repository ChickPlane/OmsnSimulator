#include "stdafx.h"
#include "TestSessionBsw.h"


CTestSessionBsw::CTestSessionBsw()
	: m_lnTimeOut(0)
{
}


CTestSessionBsw::CTestSessionBsw(const CTestSessionBsw & src)
{
	*this = src;
}

CTestSessionBsw & CTestSessionBsw::operator=(const CTestSessionBsw & src)
{
	CTestSession::operator=(src);
	m_lnTimeOut = src.m_lnTimeOut;
	return *this;
}

CTestSessionBsw::~CTestSessionBsw()
{
}
