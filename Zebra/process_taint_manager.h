#ifndef _PROCESS_TAINT_MANAGER_H_
#define _PROCESS_TAINT_MANAGER_H_

#include "taint_manager.h"

class ProcessTaintManager
{
protected:
	PIN_LOCK		m_lock;
	TaintManager	m_taint_manager;

public:
	ProcessTaintManager();

	// 레지스터 관련 함수 
	VOID DoTaintRegister(REG reg, Taint& taint);
	VOID UnTaintRegister(REG reg);
	BOOL IsTaintedRegister(REG reg, Taint& taint);

	// 메모리 관련 함수
	VOID DoTaintMemory(ADDRINT addr, UINT32 size, Taint& taint);
	VOID UnTaintMemory(ADDRINT addr, UINT32 size);
	BOOL IsTaintedMemory(ADDRINT addr, UINT32 size, Taint& taint);
};

#endif // _PROCESS_TAINT_MANAGER_H_