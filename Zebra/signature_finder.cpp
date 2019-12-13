#include "signature_finder.h"
#include "function_callback.h"
#include "instrument.h"

SignatureFinder::SignatureFinder()
{
	// __alloca_probe_16
	m_signatures.push_back(Signature("\x51\x8D\x4C\x24\x08\x2B\xC8\x83\xE1\x0F\x03\xC1\x1B\xC9\x0B\xC1\x59", 17, InstrumentFunction));
	PIN_InitLock(&m_lock);
}

BOOL SignatureFinder::Match(INS ins)
{
	PIN_GetLock(&m_lock, PIN_ThreadId());
	vector<Signature>::iterator it;
	BOOL result = FALSE;

	for (it = m_signatures.begin(); it != m_signatures.end(); it++)
	{
		if ((*it).Match(ins))
		{
			result = TRUE;
			cerr << "[*] SignatureFinder::Match - " << hex << INS_Address(ins) << "@" << IMG_Name(SEC_Img(RTN_Sec(INS_Rtn(ins)))) << endl;
		}
	}
	PIN_ReleaseLock(&m_lock);
	return FALSE;
}

