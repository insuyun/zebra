#ifndef _SIGNAUTE_FINDER_H_
#define _SIGNAUTE_FINDER_H_

#include "signature.h"

class SignatureFinder
{
private:
	vector<Signature> m_signatures;
	PIN_LOCK		m_lock;

public:
	SignatureFinder();
	BOOL Match(INS ins);
};

#endif //_SIGNAUTE_FINDER_H_