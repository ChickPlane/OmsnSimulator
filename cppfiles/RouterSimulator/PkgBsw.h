#pragma once
#include "Sentence.h"
class CRoutingProtocol;
class CTestSession;

typedef unsigned int BSW_USERID;

class CPkgBswData : public CSentence
{
public:
	CPkgBswData();
	CPkgBswData(const CPkgBswData & src);
	virtual CPkgBswData & operator=(const CPkgBswData & src);
	virtual ~CPkgBswData();
	virtual BOOL IsReceiver(BSW_USERID uId) const;
	BOOL IsKnownNode(CRoutingProtocol * pTest) const;
	virtual CTestSession * DeepCopySession() const;
	void HalfCount(BOOL bBottom);
	int GetHalfCopyCount(BOOL bBottom) const;
	void MergeMessage(const CPkgBswData & src);

	void InitParameters(int nCopyCount, BSW_USERID uReceiverId);
	BOOL OnlyOneCopyLeft();
	BSW_USERID GetReceiverId() const;

	CList<CRoutingProtocol *> m_KnownNodes;
	int m_nBswId;
	CTestSession * m_pTestSession;
	BOOL m_bLastHop;

protected:
	int m_nCopyCount;
	BSW_USERID m_uReceiverId;

private:
	void ChangeId();
	static int sm_nBswIdMax;
};

