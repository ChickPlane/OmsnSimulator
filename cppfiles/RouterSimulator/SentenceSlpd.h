#pragma once
#include "Sentence.h"
#include "TestSession.h"
#include "TimeOutPair.h"
#include "SimulatorCommon.h"
#include "PkgBsw.h"

typedef USERID SLPD_USERID;

class CSlpdUserAndPseudo
{
public:
	SLPD_USERID m_lnUserId;
	SLPD_USERID m_lnPseudonym;
};

enum {
	SLPD_SENTENCE_TYPE_DATA = SENTENCE_TYPE_MAX,
	SLPD_SENTENCE_TYPE_MAX
};

class CPkgSlpd :
	public CSentence
{
public:
	CPkgSlpd();
	CPkgSlpd(const CPkgSlpd & src);
	CPkgSlpd & operator = (const CPkgSlpd & src);
	virtual ~CPkgSlpd();

	virtual SLPD_USERID GetOriginalRequester() const;
	virtual BOOL IsAlreadyInFriendList(SLPD_USERID uId) const;

	SLPD_USERID m_nPseudonym;
	int m_nRemainTimes;
	SIM_TIME m_lnTimeout;
	CList<SLPD_USERID> m_FriendList;
	SLPD_USERID m_RecverId;
};

class CPkgSlpdAck : public CPkgAck
{
public:
	CPkgSlpdAck();
	CPkgSlpdAck(const CPkgSlpdAck & src);
	CPkgSlpdAck & operator=(const CPkgSlpdAck & src);
	~CPkgSlpdAck();
	virtual void CleanIds();
	virtual void SetIds(const CList<CTimeOutPair<CSlpdUserAndPseudo>> & pseduonymPairs);
	virtual BOOL IsInId(SLPD_USERID nID);

	SLPD_USERID * m_pIDs;
	int m_nIDNumber;
};

class CPkgSlpdReply : public CPkgBswData
{
public:
	CPkgSlpdReply();
	CPkgSlpdReply(const CPkgSlpdReply & src);
	CPkgSlpdReply & operator=(const CPkgSlpdReply & src);
	~CPkgSlpdReply();

	BOOL m_bIsPseudonym;
};