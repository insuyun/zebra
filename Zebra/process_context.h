#ifndef _PROCESS_CONTEXT_H_
#define _PROCESS_CONTEXT_H_

#include "defines.h"
#include "handle_manager.h"
#include "argument_storage.h"
#include "thread_context.h"
#include "taint.h"
#include "process_taint_manager.h"
#include "signature_finder.h"

class ProcessContext
{
protected:
	ThreadContext			m_threads[MAX_THREADS];
	HandleManager			m_handle_manager;
	ProcessTaintManager		m_process_taint_manager;
	SignatureFinder			m_signature_finder;

	// 디버깅용 변수
	BOOL					m_taint_started;
public:
	ProcessContext();

	// Process 관련 함수
	VOID SaveHandle(HANDLE hFile);
	VOID SaveMappingHandle(HANDLE hFileMapping);

	BOOL ExistHandle(HANDLE hFile);
	BOOL ExistMappingHandle(HANDLE hFile);

	// Thread 관련 함수 
	VOID SaveArgument(THREADID thread_id, FUNCTION_TYPE type, Argument* arg);
	Argument* ReadArgument(THREADID thread_id, FUNCTION_TYPE type);
	VOID DeleteArgument(THREADID thread_id, FUNCTION_TYPE type);
	BOOL ExistArgument(THREADID thread_id, FUNCTION_TYPE type);

	VOID ThreadStart(THREADID thread_id, ADDRINT esp);
	VOID ThreadFini(THREADID thread_id);

	// 테인트 관련 함수
	VOID DoTaintRegister(THREADID thread_id, REG reg, Taint& taint);
	VOID DoTaintMemory(THREADID thread_id, ADDRINT addr, UINT32 size, Taint& taint);

	VOID UnTaintRegister(THREADID thread_id, REG reg);
	VOID UnTaintMemory(THREADID thread_id, ADDRINT addr, UINT32 size);

	BOOL IsTaintedRegister(THREADID thread_id, REG reg, Taint& taint);
	BOOL IsTaintedMemory(THREADID thread_id, ADDRINT addr, UINT32 size, Taint& taint);

	// 디버그 관련 함수 
	VOID Dump();

	VOID DefineTaintSource(THREADID thread_id, ADDRINT addr, UINT32 size, UINT32 offset);
};

#endif // _PROCESS_CONTEXT_H_