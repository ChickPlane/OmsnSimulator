#include "stdafx.h"
#include "Sentence.h"


CSentence::CSentence()
	: m_nSentenceType(SENTENCE_TYPE_NULL)
	, m_pSender(NULL)
	, m_pSpeakTo(NULL)
	, m_nProcessID(-1)
{
}


CSentence::CSentence(const CSentence & src)
{
	*this = src;
}

CSentence & CSentence::operator=(const CSentence & src)
{
	m_nSentenceType = src.m_nSentenceType;
	m_pSender = src.m_pSender;
	m_pSpeakTo = src.m_pSpeakTo;
	m_nProcessID = src.m_nProcessID;
	return *this;
}

CSentence::~CSentence()
{
}
