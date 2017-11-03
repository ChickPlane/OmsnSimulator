#pragma once
#include "Sentence.h"

enum {
	MH_SENTENCE_TYPE_DATA = SENTENCE_TYPE_MAX,
	MH_SENTENCE_TYPE_MAX
};

class CPkgMultiHop :
	public CSentence
{
public:
	CPkgMultiHop();
	CPkgMultiHop(const CPkgMultiHop & src);
	CPkgMultiHop & operator = (const CPkgMultiHop & src);
	virtual ~CPkgMultiHop();

	USERID m_nFromId;
	USERID m_nToId;
	SIM_TIME m_lnTimeout;
};

