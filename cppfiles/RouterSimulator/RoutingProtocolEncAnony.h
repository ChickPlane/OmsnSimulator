#pragma once
#include "RoutingProtocolBSW.h"
#include "TrustValue.h"
#include "RouteTable.h"
#include "RoutingProcessPmhns.h"
#include "DoublePoint.h"
#include "RoutingDataEnc.h"

class CRoutingMsgEncAnony;
class CRoutingProcessBSW;

enum
{
	EA_TIMER_CMD_FRIEND_SEARCH,
	EA_TIMER_CMD_MAX
};

class CEncAnonyPacket : public CRoutingDataEnc
{
public:
	CEncAnonyPacket();
	CEncAnonyPacket(const CEncAnonyPacket & src);
	virtual CEncAnonyPacket & operator = (const CEncAnonyPacket & src);
	virtual CEncAnonyPacket * GetDuplicate() const;


	double GetDistance(const CDoublePoint & test);
	CDoublePoint lastPosition;
	double fRp;
	double fRs;
	double fTrustBound;
	double fMaxCost;
	double fUsedCost;
	bool bHolding;
	CRoutingProtocol * pLastHop;

	void ChangeValue(double uc);
};

class CRoutingProtocolEncAnony :
	public CRoutingProtocol, public CRoutingUserProcessPmhns
{
public:
	CRoutingProtocolEncAnony();
	~CRoutingProtocolEncAnony();

	void SetPrivacyParam(double fTrustBound, double fRp, double fRs, double fCostMax);
	virtual void SetCopyCount(int nCopyCount);

	virtual void OnReceivedMsg(const CRoutingMsg * pMsg);
	virtual void SendPackage(const CRoutingDataEnc & encData);
	virtual void OnEngineTimer(int nCommandId);

	virtual void CleanTimeOutData();

	virtual void StartFriendSearch();
	virtual void CheckFriendSearch();
	virtual void CheckPosition();

	virtual void OnGetPmhnsRep(const CList<CRoutingProtocol*> & path);
	virtual void OnGetPmhnsData(const CRoutingMsg * pMsg);

	virtual void SendAnonymityData(const CTrustValue & tv, const CList<CRoutingProtocol*> & path);
	virtual void SendFreeData(const CRoutingMsg & Msg);

	virtual int GetInfoList(CList<CString> & ret);
	virtual void GetAllCarryingMessages(CList<CRoutingDataEnc> & ret) const;

	virtual int GetInportantLevel() const;

	CRouteTable m_RouteTable;
	CList<CEncAnonyPacket *> m_AnonymityList;
	CTrustValue TrustFunction(double fDistance);
	CRoutingProcessBSW * m_pBSW;
	CRoutingProcessPmhns * m_pPmhns;

protected:
	bool m_bInSearching;

	int m_nPmhnsId;
	int m_nBswId;

	double m_fTrustBound;
	double m_fRp;
	double m_fRs;
	double m_fCostMax;
};

