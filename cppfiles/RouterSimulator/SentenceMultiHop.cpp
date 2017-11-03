#include "stdafx.h"
#include "SentenceMultiHop.h"


CPkgMultiHop::CPkgMultiHop()
	: m_nFromId(UID_INVALID)
	, m_nToId(UID_INVALID)
	, m_lnTimeout(0)
{
	m_nSentenceType = MH_SENTENCE_TYPE_DATA;
}


CPkgMultiHop::CPkgMultiHop(const CPkgMultiHop & src)
{
	*this = src;
}

CPkgMultiHop & CPkgMultiHop::operator=(const CPkgMultiHop & src)
{
	CSentence::operator=(src);
	m_nFromId = src.m_nFromId;
	m_nToId = src.m_nToId;
	m_lnTimeout = src.m_lnTimeout;
	return *this;
}

CPkgMultiHop::~CPkgMultiHop()
{
}
