#include "stdafx.h"
#include "TrustValue.h"


CTrustValue::CTrustValue()
{
}


CTrustValue::CTrustValue(const CTrustValue & src)
{
	*this = src;
}

CTrustValue::CTrustValue(double fValue)
{
	m_fValue = fValue;
}

CTrustValue & CTrustValue::operator=(const CTrustValue & src)
{
	m_fValue = src.m_fValue;
	return *this;
}

CTrustValue & CTrustValue::operator=(double src)
{
	m_fValue = src;
	return *this;
}

CTrustValue::~CTrustValue()
{
}

CTrustValue::operator double() const
{
	int nRet = m_fValue * TRUST_VALUE_ACCURACY + 0.5;
	double fTmp = nRet / TRUST_VALUE_ACCURACY;
	return fTmp;
}

int CTrustValue::GetIntValue() const
{
	int nRet = m_fValue * TRUST_VALUE_ACCURACY + 0.5;
	return nRet;
}

bool CTrustValue::operator<=(const CTrustValue & other) const
{
	int nThis = this->GetIntValue();
	int nOther = other.GetIntValue();
	return nThis <= nOther;
}

bool CTrustValue::operator==(const CTrustValue & other) const
{
	int nThis = this->GetIntValue();
	int nOther = other.GetIntValue();
	return nThis == nOther;
}

bool CTrustValue::operator<(const CTrustValue & other) const
{
	int nThis = this->GetIntValue();
	int nOther = other.GetIntValue();
	return nThis < nOther;
}
