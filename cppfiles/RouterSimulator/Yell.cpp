#include "stdafx.h"
#include "Yell.h"
#include "Sentence.h"


CYell::CYell()
	: m_pSender(NULL)
	, m_pRecver(NULL)
	, m_ppSentences(NULL)
	, m_nSentenceCount(0)
{
}


CYell::CYell(const CYell & src)
{
	*this = src;
}

CYell & CYell::operator=(const CYell & src)
{
	Clean();
	m_pSender = src.m_pSender;
	m_pRecver = src.m_pRecver;
	m_nSentenceCount = src.m_nSentenceCount;

	if (m_nSentenceCount > 0)
	{
		m_ppSentences = new CSentence*[m_nSentenceCount];
		memcpy(m_ppSentences, src.m_ppSentences, sizeof(CSentence*)*m_nSentenceCount);
	}
	else
	{
		m_ppSentences = NULL;
	}
	return *this;
}

CYell::~CYell()
{
	Clean();
}

void CYell::SetSentenceLength(int nLength)
{
	Clean();
	m_nSentenceCount = nLength;
	m_ppSentences = new CSentence *[nLength];
}

CRoutingProtocol * CYell::SetSentences(CList<CSentence*> & Sentences)
{
	CRoutingProtocol * pTo = NULL;
	BOOL bSingleTarget = TRUE;
	int nSentenceLen = Sentences.GetSize();
	SetSentenceLength(nSentenceLen);
	int i = 0;
	POSITION pos = Sentences.GetHeadPosition();
	while (pos)
	{
		CSentence * pSentence = Sentences.GetNext(pos);
		m_ppSentences[i++] = pSentence;
		if (bSingleTarget)
		{
			if (pSentence->m_pProtocol != NULL)
			{
				if (pTo == NULL)
				{
					pTo = pSentence->m_pProtocol;
				}
				else if (pTo != pSentence->m_pProtocol)
				{
					bSingleTarget = FALSE;
				}
			}
			else
			{
				bSingleTarget = FALSE;
			}
		}
	}
	if (bSingleTarget == FALSE)
	{
		pTo = NULL;
	}
	return pTo;
}

void CYell::Clean()
{
	if (m_nSentenceCount > 0)
	{
		for (int i = 0; i < m_nSentenceCount; ++i)
		{
			delete m_ppSentences[i];
		}
		delete[] m_ppSentences;
		m_ppSentences = NULL;
		m_nSentenceCount = 0;
	}
}
