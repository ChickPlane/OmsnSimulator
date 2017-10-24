#include "stdafx.h"
#include "TestSessionBsw.h"


CTestSessionBsw::CTestSessionBsw()
{
}


CTestSessionBsw::CTestSessionBsw(const CTestSessionBsw & src)
{
	*this = src;
}

CTestSessionBsw & CTestSessionBsw::operator=(const CTestSessionBsw & src)
{
	CTestSession::operator=(src);
	return *this;
}

CTestSessionBsw::~CTestSessionBsw()
{
}
