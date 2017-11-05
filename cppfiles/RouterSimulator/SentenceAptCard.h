#pragma once
#include "Sentence.h"
#include "PkgBsw.h"
#include "TestSessionAptCard.h"
#include "AppointmentCard.h"

enum {
	AC_SENTENCE_TYPE_CARD = SENTENCE_TYPE_MAX,
	AC_SENTENCE_TYPE_MAX
};

class CPkgAptCardHello : public CPkgHello
{
public:
	CPkgAptCardHello();
	CPkgAptCardHello(const CPkgAptCardHello & src);
	~CPkgAptCardHello();
};

class CPkgAptCardAck : public CPkgAck
{
public:
	CPkgAptCardAck();
	CPkgAptCardAck(const CPkgAptCardAck & src);
	CPkgAptCardAck & operator=(const CPkgAptCardAck & src);
	~CPkgAptCardAck();
	virtual void CleanIds();
	virtual void SetIds(const CList<CPseudonymPairRecord> & pseduonymPairs);

	BOOL m_bAskForCards;
	CPseudonymPair * m_pIDs;
	int m_nIDNumber;
	int m_nHoldingTrustNumber;
};

class CPkgAptCardQuery : public CPkgBswData
{
public:
	CPkgAptCardQuery();
	CPkgAptCardQuery(const CPkgAptCardQuery & src);
	CPkgAptCardQuery & operator=(const CPkgAptCardQuery & src);
	~CPkgAptCardQuery();

	virtual CTestSessionAptCard * DeepCopySession() const;

	USERID m_nAid;
	int m_nAapt;
};

class CPkgAptCardReply : public CPkgBswData
{
public:
	CPkgAptCardReply();
	CPkgAptCardReply(const CPkgAptCardReply & src);
	CPkgAptCardReply & operator=(const CPkgAptCardReply & src);
	~CPkgAptCardReply();

	virtual CTestSessionAptCard * DeepCopySession() const;

	CPseudonymPair m_Next;
	// The destination is a real ID.
	BOOL m_bCheckId;
};

class CPkgAptCardCards : public CSentence
{
public:
	CPkgAptCardCards();
	CPkgAptCardCards(const CPkgAptCardCards & src);
	CPkgAptCardCards & operator=(const CPkgAptCardCards & src);
	~CPkgAptCardCards();

	virtual void CleanCards();
	virtual void SetCardCount(int nCardNumber);
	virtual void SetCards(CList<CAppointmentCard *> & SendingList);
	BOOL HasDuplicated();
	CString GetString() const;

	CAppointmentCard * m_pCards;
	int m_nCardNumber;
};