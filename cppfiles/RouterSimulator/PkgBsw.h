#pragma once
#include "Sentence.h"
#include "TestSessionBsw.h"
class CRoutingProtocol;

typedef unsigned int BSW_USERID;

enum
{
	BSW_MSG_TYPE_DATA = SENTENCE_TYPE_MAX + 23,
	BSW_MSG_TYPE_MAX
};

class CPkgBswData : public CSentence
{
public:
	CPkgBswData();
	CPkgBswData(const CPkgBswData & src);
	virtual CPkgBswData & operator=(const CPkgBswData & src);
	virtual ~CPkgBswData();
	BOOL IsKnownNode(const CRoutingProtocol * pTest) const;
	virtual CTestSessionBsw * DeepCopySession() const;
	void HalfCount(BOOL bBottom);
	int GetHalfCopyCount(BOOL bBottom) const;
	void MergeMessage(const CPkgBswData & src);
	BOOL OnlyOneCopyLeft();
	BSW_USERID GetReceiverId() const;
	void ChangeId();


	CList<const CRoutingProtocol *> m_KnownNodes;
	int m_nBswId;
	CTestSessionBsw * m_pTestSession;
	BOOL m_bLastHop;
	SIM_TIME m_lnTimeOut;
	int m_nCopyCount;
	BSW_USERID m_uReceiverId;
	BSW_USERID m_uSenderId;

private:
	static int sm_nBswIdMax;
};

