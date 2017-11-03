#pragma once
#include "RoutingProtocol.h"
#include "RoutingProcessBSW.h"
#include "RoutingProcessHello.h"
#include "TestRecordBsw.h"
class CRoutingProtocolBsw :
	public CRoutingProtocol, public CRoutingProcessBswUser, public CRoutingProcessHelloUser
{
public:
	CRoutingProtocolBsw();
	virtual ~CRoutingProtocolBsw();

	virtual void CreateQueryMission(const CQueryMission * pMission);
	virtual void SetLocalParameters(int nBswCopyCount);
	virtual void Turn(BOOL bOn);
	virtual COLORREF GetImportantLevel() const;
	virtual void OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev);

public:
	// Hello User
	virtual void OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg);

	// BSW User
	virtual BOOL IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual CPkgBswData * CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual void OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual void OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);

protected:
	CRoutingProcessBsw * GetQueryProcess() const;
	CRoutingProcessBsw * GetReplyProcess() const;
	CRoutingProcessHello * GetHelloProcess() const;
	void ResetAll();
	BOOL SetMissionRecord(int nSessionId, int nEventId);

public:
	static CMap<int, int, CTestRecordBsw *, CTestRecordBsw *> gm_allSessions;
	void SetMissionForwardNumber(int nSessionId, int nForwardNumber);
	void UpdateSummary();

private:
	int m_nHelloProcessId;
	int m_nQueryBswProcessId;
	int m_nReplyBswProcessId;

};

