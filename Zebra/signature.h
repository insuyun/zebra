#ifndef _SIGNATURE_H_
#define _SIGNATURE_H_
#include "defines.h"

class Signature
{
private:
	char*		m_pattern;
	UINT32		m_pattern_length;
	VOID		(*m_callback)(INS ins);

public:
	Signature(char* pattern, UINT32 pattern_length, VOID (*callback)(INS ins))
	{
		m_pattern = pattern;
		m_pattern_length = pattern_length;
		m_callback = callback;
	}

	BOOL Match(INS ins)
	{
		ADDRINT addr = INS_Address(ins);
		VOID*	ptr	 = malloc(m_pattern_length);

		PIN_SafeCopy(ptr, (VOID*)addr, m_pattern_length);

		if (!memcmp(ptr, m_pattern, m_pattern_length))
		{
			m_callback(ins);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};

#endif //_SIGNATURE_H_