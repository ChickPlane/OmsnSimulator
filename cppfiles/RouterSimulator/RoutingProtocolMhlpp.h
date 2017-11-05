#pragma once
#include "RoutingProtocol.h"
#include "RoutingProcessMultiHop.h"
#include "RoutingProcessHello.h"
#include "RoutingProcessBSW.h"
#include "PkgMhlpp.h"
#include "TimeOutPair.h"
#include "TestRecordMhlpp.h"

class CRoutingProtocolMhlpp :
	public CRoutingProtocol,
	public CRoutingProcessMultiHopUser,
	public CRoutingProcessHelloUser,
	public CRoutingProcessBswUser
{
public:
	CRoutingProtocolMhlpp();
	virtual ~CRoutingProtocolMhlpp();

	virtual void CreateQueryMission(const CQueryMission * pMission);
	static void SetStaticParameters(double fTrust, double fObfuscationRadius);
	virtual void SetLocalParameters(int nBswCopyCount);
	virtual COLORREF GetImportantLevel() const;
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual void Turn(BOOL bOn);
	virtual int GetDebugNumber(int nParam) { return 0; }
	CString GetDebugString() const { return _T(""); }
	virtual void OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev);

	// MultiHop User
	virtual CRoutingProtocol * GetNextHop(CRoutingProcessMultiHop * pCallBy, USERID nDestinationId);
	virtual CPkgMultiHop * GetMultiHopDataCopy(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg);
	virtual void OnMultiHopMsgArrived(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg);

	// Hello User
	virtual void OnHearHelloFromOthers(CRoutingProcessHello * pCallBy, const CPkgHello * pPkg) {};
	virtual void OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg);
	virtual CPkgAck * GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo);

	// BSW User
	virtual BOOL IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger);
	virtual BOOL IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual CPkgBswData * CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual void OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual void OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);

protected:
	virtual CPkgMhlppReply * ForwardToOriginal(const CPkgMhlppReply * pReply);
	virtual BOOL IsTrustful(int nHostId) const;

	static CPkgMhlppReply * LbsPrepareReply(const CPkgBswData * pQuery);
	BOOL TryToFinishObfuscation(const CPkgMhlpp & ObfuscatingPkg);
	BOOL IsFartherThanMe(const CPkgMhlpp & ObfuscatingPkg, CHost * pTheOther);
	virtual BOOL IsInPseudonymList(USERID lnPseudonym, CMhlppUserAndPseudo & ret);
	virtual void OnMulNeighbourDifferent(const CMsgCntJudgeReceiverReport* pWholeReport);

private:
	CRoutingProcessMultiHop * GetMhProcess() const;
	CRoutingProcessBsw * GetQueryProcess() const;
	CRoutingProcessBsw * GetReplyProcess() const;
	CRoutingProcessHello * GetHelloProcess() const;

private:
	CList<CTimeOutPair<CPkgMhlpp>> m_WaitingList;
	CList<CTimeOutPair<CMhlppUserAndPseudo>> m_Pseudonyms;
	int m_nHelloProcessId;
	int m_nQueryBswProcessId;
	int m_nReplyBswProcessId;
	int m_nMhProcessId;
	static double gm_fTrust;
	static double gm_fObfuscationRadius;
	static int gm_nPseudonymMax;
};

