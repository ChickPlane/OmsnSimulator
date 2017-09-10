#include "stdafx.h"
#include "HostGui.h"


CHostGui::CHostGui()
	: m_pHost(NULL)
{
}


CHostGui::CHostGui(const CHostGui & src)
{
	*this = src;
}

CHostGui::~CHostGui()
{
}

CHostGui & CHostGui::operator=(const CHostGui & src)
{
	m_pHost = src.m_pHost;
	m_Position = src.m_Position;
	return *this;
}
