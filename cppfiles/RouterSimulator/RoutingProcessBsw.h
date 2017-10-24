#pragma once
#include "RoutingProcess.h"
#include "PkgBsw.h"

class CRoutingProcessBsw;

class CBswIdAndTimeout
{
public:
	int m_nBswId;
	SIM_TIME m_lnTimeout;
};

class CRoutingProcessBswUser
{
public:
	virtual BOOL IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger);
	virtual BOOL IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg) = 0;
	virtual CPkgBswData * CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg) = 0;

	virtual void OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg) {}
	virtual void OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg) {}
};

class CRoutingProcessBsw :
	public CRoutingProcess
{
public:
	CRoutingProcessBsw();
	virtual ~CRoutingProcessBsw();

	virtual void SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol);
	virtual void SetProcessUser(CRoutingProcessBswUser * pUser) { m_pUser = pUser; }
	virtual void OnEncounterUser(CRoutingProtocol * pTheOther, CList<CSentence *> & SendingList, const CSentence * pTriger);
	virtual void OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList);

	virtual void InitNewPackage(CPkgBswData * pPkg, BSW_USERID uReceiverId, SIM_TIME lnTimeOut);
	virtual void InitNewPackage(CPkgBswData * pPkg);
	virtual void SetCopyCount(int nCopyCount);

	virtual int GetDataMapSize() const;

	virtual void InsertToDataMap(CPkgBswData* pPkg);

protected:
	virtual void CleanTimeoutPkgRecord();
	virtual void CleanTimeoutData();

	virtual CPkgBswData * CheckDuplicateInMap(int nBswId);
	virtual void InsertToDestinationRecvRecord(const CBswIdAndTimeout & BT);
	static void InsertToBTList(const CBswIdAndTimeout & newItem, CList<CBswIdAndTimeout> & BTList);
	
	virtual BOOL IsDestRecvRecordExist(int nBswId);

private:
	CMap<int, int, CPkgBswData*, CPkgBswData*> m_DataMap;
	CList<CBswIdAndTimeout> m_DataTimeoutList;

	CList<CBswIdAndTimeout> m_PkgRecvRecord;

	int m_nCopyCount;
	CRoutingProcessBswUser * m_pUser;
};

