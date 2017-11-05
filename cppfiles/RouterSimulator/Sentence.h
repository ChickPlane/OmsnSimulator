#pragma once
#include "TestSession.h"

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

	virtual CTestSession * DeepCopySession() const;
	virtual BOOL IsInStatistic() const;

	int m_nProcessID;
	int m_nSentenceType;
	CRoutingProtocol * m_pSender;
	CRoutingProtocol * m_pSpeakTo;
	CTestSession * m_pTestSession;
};

class CPkgHello : public CSentence
{
public:
	CPkgHello() { m_nSentenceType = SENTENCE_TYPE_HELLO; }
	CPkgHello(const CPkgHello & src) { *this = src; }
	virtual ~CPkgHello() {};
};

class CPkgAck : public CSentence
{
public:
	CPkgAck() { m_nSentenceType = SENTENCE_TYPE_ACK; }
	CPkgAck(const CPkgAck & src) { *this = src; }
	void SetAsOkPackage() { m_nSentenceType = SENTENCE_TYPE_OK; }
};
