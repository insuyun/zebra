#ifndef _THREAD_CONTEXT_H_
#define _THREAD_CONTEXT_H_

#include "argument_storage.h"
#include "taint_manager.h"

class ThreadContext 
{
protected:
	ArgumentStorage		m_arg_storage;
	TaintManager		m_taint_manager;
	ADDRINT				m_stack_base_addr;
	

public:
	VOID SaveArgument(FUNCTION_TYPE type, Argument* arg);
	Argument* ReadArgument(FUNCTION_TYPE type);
	VOID DeleteArgument(FUNCTION_TYPE type);
	BOOL ExistArgument(FUNCTION_TYPE type);

	VOID SetStackBaseAddress(ADDRINT stack_base_addr);
	ADDRINT GetStackBaseAddress();

	VOID DoTaintMemory(ADDRINT addr, UINT32 size, Taint& taint);
	VOID DoTaintRegister(REG reg, Taint& taint);

	VOID UnTaintMemory(ADDRINT addr, UINT32 size);
	VOID UnTaintRegister(REG reg);

	BOOL IsTaintedMemory(ADDRINT addr, UINT32 size, Taint& taint);
	BOOL IsTaintedRegister(REG reg, Taint& taint);

	BOOL IsStackAddress(ADDRINT addr);

	// 디버그 관련 함수
	VOID Dump();
};

#endif //_THREAD_CONTEXT_H_