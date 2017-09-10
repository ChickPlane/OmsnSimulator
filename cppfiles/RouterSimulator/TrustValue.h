#pragma once

#define TRUST_VALUE_ACCURACY 10000000.0

class CTrustValue
{
public:
	CTrustValue();
	CTrustValue(double fValue);
	CTrustValue(const CTrustValue & src);
	CTrustValue & operator = (const CTrustValue & src);
	CTrustValue & operator = (double src);
	virtual ~CTrustValue();

	operator double() const;
	bool operator < (const CTrustValue & other) const;
	bool operator == (const CTrustValue & other) const;
	bool operator <= (const CTrustValue & other) const;
	int GetIntValue() const;

	double m_fValue;
};

