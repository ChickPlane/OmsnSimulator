#pragma once
#include "RoutingData.h"
class CRoutingDataEnc :
	public CRoutingData
{
public:
	CRoutingDataEnc();
	CRoutingDataEnc(const CRoutingDataEnc & src);
	virtual CRoutingDataEnc & operator = (const CRoutingDataEnc & src);
	virtual ~CRoutingDataEnc();
	virtual CRoutingDataEnc * GetDuplicate() const;

	virtual int GetDataId(const CHost * pReader) const;
	virtual CHost * GetHostFrom(const CHost * pReader) const;
	virtual CHost * GetHostTo(const CHost * pReader) const;
	virtual SIM_TIME GetTimeOut(const CHost * pReader) const;

	virtual bool CanRead(const CHost * pReader) const;
	virtual void SetEncrypted(bool bEnc);

	virtual int ForceGetDataId(const CHost * pReader) const;
	virtual CHost * ForceGetHostFrom(const CHost * pReader) const;
	virtual CHost * ForceGetHostTo(const CHost * pReader) const;
	virtual SIM_TIME ForceGetTimeOut(const CHost * pReader) const;

protected:
	bool m_bIsEncrypted;
};

