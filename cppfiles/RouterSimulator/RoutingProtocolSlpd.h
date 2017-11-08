#pragma once
#include "RoutingProtocol.h"
#include "RoutingProcessSlpd.h"
#include "RoutingProcessHello.h"
#include "RoutingProcessBSW.h"
#include "TestRecordSlpd.h"

class CRoutingProtocolSlpd :
	public CRoutingProtocol,
	public CRoutingProcessSlpdUser,
	public CRoutingProcessHelloUser,
	public CRoutingProcessBswUser
{
public:
	CRoutingProtocolSlpd();
	virtual ~CRoutingProtocolSlpd();

	virtual void CreateQueryMission(const CQueryMission * pMission);
	static void SetStaticParameters(int nK, double fTrust);
	virtual void SetLocalParameters(int nBswCopyCount);
	virtual COLORREF GetImportantLevel() const;
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual void Turn(BOOL bOn);
	virtual int GetCarryingPkgNumber(int nParam);
	CString GetDebugString() const;
	virtual void OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev);

	// SLPD
	virtual BOOL IsTrustful(CRoutingProcessSlpd * pCallBy, const CRoutingProtocol * pOther) const;
	virtual void OnNewSlpdPseudoOver(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg);
	virtual void OnFirstSlpdObfuscationForward(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg);

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
	virtual CPkgSlpdReply * ForwardToOriginal(const CPkgSlpdReply * pReply);

	static CPkgSlpdReply * LbsPrepareReply(const CPkgBswData * pQuery);

private:
	CRoutingProcessSlpd * GetSlpdProcess() const;
	CRoutingProcessBsw * GetQueryProcess() const;
	CRoutingProcessBsw * GetReplyProcess() const;
	CRoutingProcessHello * GetHelloProcess() const;

private:
	int m_nHelloProcessId;
	int m_nQueryBswProcessId;
	int m_nReplyBswProcessId;
	int m_nSlpdProcessId;
	static double gm_fTrust;
};

