#pragma once
class CRoutingProtocol;
class CSentence;

class CYell
{
public:
	CYell();
	CYell(const CYell & src);
	CYell & operator=(const CYell & src);
	virtual ~CYell();

	void SetSentenceLength(int nLength);
	CRoutingProtocol * SetSentences(CList<CSentence*> & Sentences);
	void Clean();
	CRoutingProtocol * m_pSender;
	// If m_pRecver is NULL, then it is a broadcast.
	CRoutingProtocol * m_pRecver;
	CSentence ** m_ppSentences;
	int m_nSentenceCount;
};

