#pragma once
#include "SimulatorCommon.h"

class CPseudonymPair
{
public:
	USERID m_nGeneratorId;
	int m_nApt;
	USERID m_nPseudonym;

	virtual CPseudonymPair & operator=(const CPseudonymPair & src)
	{
		m_nGeneratorId = src.m_nGeneratorId;
		m_nApt = src.m_nApt;
		m_nPseudonym = src.m_nPseudonym;
		return *this;
	}

	BOOL IsRealName() const
	{
		return m_nGeneratorId == UID_LBSP0;
	}

	bool operator == (const CPseudonymPair & src) const
	{
		return EqualTo(src.m_nGeneratorId, src.m_nApt, src.m_nPseudonym);
	}
	BOOL EqualTo(USERID AAid, int AApt, USERID recvId) const
	{
		if (m_nGeneratorId != AAid)
		{
			return FALSE;
		}
		if (m_nApt != AApt)
		{
			return FALSE;
		}
		if (m_nPseudonym != recvId)
		{
			;
		}
		return TRUE;
	}
};

class CPseudonymPairRecord : public CPseudonymPair
{
public:
	SIM_TIME m_lnTimeOut;

	virtual CPseudonymPairRecord & operator=(const CPseudonymPairRecord & src)
	{
		m_lnTimeOut = src.m_lnTimeOut;
		CPseudonymPair::operator=(src);
		return *this;
	}
};

#define INVALID_APT_CARD_NUMBER -1

class CAppointmentCard
{
public:
	CAppointmentCard();
	CAppointmentCard(const CAppointmentCard & src);
	virtual ~CAppointmentCard();

	virtual CAppointmentCard & operator = (const CAppointmentCard & src);
	BOOL IsFinal() const;
	BOOL CheckDuplicatedUid(USERID uUid) const;
	CString GetString() const;

	USERID m_nCid;
	int m_nCapt;
	USERID m_nAid;
	int m_nAapt;
	SIM_TIME m_lnTimeout;
	// Add User Before leave.
	CList<USERID> m_EQ;
	CList<int> m_Mark;
	int m_nDC;
	USERID m_nPsd;
	BOOL m_bIsReady;
	SIM_TIME m_ReceiveTime;
};

