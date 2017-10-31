#include "stdafx.h"
#include "Sentence.h"


CSentence::CSentence()
	: m_nSentenceType(SENTENCE_TYPE_NULL)
	, m_pSender(NULL)
	, m_pSpeakTo(NULL)
	, m_nProcessID(-1)
	, m_bInStatistic(TRUE)
	, m_pTestSession(NULL)
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
	m_pTestSession = src.DeepCopySession();
	return *this;
}

CSentence::~CSentence()
{
}

CTestSession * CSentence::DeepCopySession() const
{
	if (m_pTestSession)
	{
		return new CTestSession(*m_pTestSession);
	}
	else
	{
		return NULL;
	}
}
