#pragma once
#include "SentenceMultiHop.h"
#include "DoublePoint.h"
#include "TimeOutPair.h"
#include "PkgBsw.h"

class CMhlppUserAndPseudo
{
public:
	USERID m_lnUserId;
	USERID m_lnPseudonym;
};

class CPkgMhlpp :
	public CPkgMultiHop
{
public:
	CPkgMhlpp();
	CPkgMhlpp(const CPkgMhlpp & src);
	CPkgMhlpp & operator = (const CPkgMhlpp & src);
	virtual ~CPkgMhlpp();

	CDoublePoint m_OriginalPosition;
	USERID m_nPseudonym;
	SIM_TIME m_lnTimeout;
	USERID m_RecverId;
	USERID m_OriginalRequesterId;
	BOOL m_bInitialed;
};


class CPkgMhlppAck : public CPkgAck
{
public:
	CPkgMhlppAck();
	CPkgMhlppAck(const CPkgMhlppAck & src);
	~CPkgMhlppAck();

	CPkgMhlppAck & operator=(const CPkgMhlppAck & src);
	virtual void CleanIds();
	virtual void SetIds(const CList<CTimeOutPair<CMhlppUserAndPseudo>> & pseduonymPairs);
	virtual BOOL IsInId(USERID nID);

	USERID * m_pIDs;
	int m_nIDNumber;
};

class CPkgMhlppReply : public CPkgBswData
{
public:
	CPkgMhlppReply();
	CPkgMhlppReply(const CPkgMhlppReply & src);
	CPkgMhlppReply & operator=(const CPkgMhlppReply & src);
	~CPkgMhlppReply();

	BOOL m_bIsPseudonym;
};