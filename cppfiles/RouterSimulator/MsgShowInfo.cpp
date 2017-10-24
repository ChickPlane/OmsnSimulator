#include "stdafx.h"
#include "MsgShowInfo.h"

CMsgShowRow & CMsgShowRow::operator=(const CMsgShowRow & src)
{
	m_Item0 = src.m_Item0;
	m_Item1 = src.m_Item1;
	m_Item2 = src.m_Item2;
	m_OtherItems.RemoveAll();
	int nLen = src.m_OtherItems.GetSize();
	m_OtherItems.SetSize(nLen);
	for (int i = 0; i < nLen; ++i)
	{
		m_OtherItems[i] = src.m_OtherItems[i];
	}
	return *this;
}


CMsgShowInfo::CMsgShowInfo()
{
}


CMsgShowInfo::~CMsgShowInfo()
{
}
