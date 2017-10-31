#pragma once

typedef enum
{
	SENTENCE_TYPE_NULL = 0,
	SENTENCE_TYPE_HELLO,
	SENTENCE_TYPE_ACK,
	SENTENCE_TYPE_OK,
	SENTENCE_TYPE_MAX
};

class CRoutingProtocol;

class CSentence
{
public:
	CSentence();
	CSentence(const CSentence & src);
	virtual CSentence & operator=(const CSentence & src);
	virtual ~CSentence();
	int m_nProcessID;
	int m_nSentenceType;
	CRoutingProtocol * m_pSender;
	CRoutingProtocol * m_pSpeakTo;

	BOOL m_bInStatistic;
};

class CPkgHello : public CSentence
{
public:
	CPkgHello() { m_nSentenceType = SENTENCE_TYPE_HELLO; m_bInStatistic = FALSE; }
	CPkgHello(const CPkgHello & src) { *this = src; }
	virtual ~CPkgHello() {};
};

class CPkgAck : public CSentence
{
public:
	CPkgAck() { m_nSentenceType = SENTENCE_TYPE_ACK; m_bInStatistic = FALSE; }
	CPkgAck(const CPkgAck & src) { *this = src; }
	void SetAsOkPackage() { m_nSentenceType = SENTENCE_TYPE_OK; }
};
