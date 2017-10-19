#include "stdafx.h"
#include "Sentence.h"


CSentence::CSentence()
	: m_nSentenceType(SENTENCE_TYPE_NULL)
	, m_pProtocol(NULL)
{
}


CSentence::CSentence(const CSentence & src)
{
	*this = src;
}

CSentence & CSentence::operator=(const CSentence & src)
{
	m_nSentenceType = src.m_nSentenceType;
	m_pProtocol = src.m_pProtocol;
	return *this;
}

CSentence::~CSentence()
{
}
