#include "process_context.h"

ProcessContext::ProcessContext()
{
	m_taint_started = FALSE;
}

VOID ProcessContext::SaveHandle(HANDLE handle)
{
	cerr << "[*] ProcessContext::SaveHandle(" << hex << handle << ")" << endl;
	m_handle_manager.SaveHandle(handle);
}
	
VOID ProcessContext::SaveMappingHandle(HANDLE handle)
{
	cerr << "[*] ProcessContext::SaveMappingHandle(" << hex << handle << ")" << endl;
	m_handle_manager.SaveMappingHandle(handle);
}

BOOL ProcessContext::ExistHandle(HANDLE handle)
{
	return m_handle_manager.ExistHandle(handle);
}

BOOL ProcessContext::ExistMappingHandle(HANDLE handle)
{
	return m_handle_manager.ExistMappingHandle(handle);
}

VOID ProcessContext::SaveArgument(THREADID thread_id, FUNCTION_TYPE type, Argument* arg)
{
	m_threads[thread_id].SaveArgument(type, arg);
}

Argument* ProcessContext::ReadArgument(THREADID thread_id, FUNCTION_TYPE type)
{
	return m_threads[thread_id].ReadArgument(type);
}

VOID ProcessContext::DeleteArgument(THREADID thread_id, FUNCTION_TYPE type)
{
	m_threads[thread_id].DeleteArgument(type);
}

BOOL ProcessContext::ExistArgument(THREADID thread_id, FUNCTION_TYPE type)
{
	return m_threads[thread_id].ExistArgument(type);
}

VOID ProcessContext::ThreadStart(THREADID thread_id, ADDRINT esp)
{
	m_threads[thread_id].SetStackBaseAddress(esp);
}

VOID ProcessContext::ThreadFini(THREADID thread_id)
{
	// 일단 지금은 아무것도 안함
}

VOID ProcessContext::DoTaintRegister(THREADID thread_id, REG reg, Taint& taint)
{
	m_threads[thread_id].DoTaintRegister(reg, taint);
}

VOID ProcessContext::DoTaintMemory(THREADID thread_id, ADDRINT addr, UINT32 size, Taint& taint)
{
	//cerr << "[*] ProcessContext::DoTaintMemory : thread_id - " << thread_id << ", addr - " << hex << addr << ", size - " << hex << size << endl;

	ThreadContext& thread_context = m_threads[thread_id];
	if (thread_context.IsStackAddress(addr))
	{
		// 스택 주소 일 경우
		thread_context.DoTaintMemory(addr, size, taint);
	}
	else
	{
		// 공유 주소일 경우
		m_process_taint_manager.DoTaintMemory(addr, size, taint);
	}
}

VOID ProcessContext::UnTaintRegister(THREADID thread_id, REG reg)
{
	m_threads[thread_id].UnTaintRegister(reg);
}
VOID ProcessContext::UnTaintMemory(THREADID thread_id, ADDRINT addr, UINT32 size)
{
	ThreadContext& thread_context = m_threads[thread_id];

	if (thread_context.IsStackAddress(addr))
	{
		thread_context.UnTaintMemory(addr, size);
	}
	else
	{
		m_process_taint_manager.UnTaintMemory(addr, size);
	}
}

BOOL ProcessContext::IsTaintedRegister(THREADID thread_id, REG reg, Taint& taint)
{
	return m_threads[thread_id].IsTaintedRegister(reg, taint);
}

BOOL ProcessContext::IsTaintedMemory(THREADID thread_id, ADDRINT addr, UINT32 size, Taint& taint)
{
	//if (m_taint_started)
	//	cerr << "[*] ProcessContext::IsTaintedMemory : thread_id - " << thread_id << ", addr - " << hex << addr << ", size - " << hex << size << endl;


	ThreadContext& thread_context = m_threads[thread_id];

	if (thread_context.IsStackAddress(addr))
	{
		return thread_context.IsTaintedMemory(addr, size, taint);
	}
	else
	{
		return m_process_taint_manager.IsTaintedMemory(addr, size, taint);
	}
}

VOID ProcessContext::Dump()
{
	cerr << "[*] ProcessContext::Dump" << endl;

	for (int i = 0; i < MAX_THREADS; i++)
	{
		cerr << "[**] Thread " << i << endl;

		m_threads[i].Dump();
	}
}

VOID ProcessContext::DefineTaintSource(THREADID thread_id, ADDRINT addr, UINT32 size, UINT32 offset)
{
	cerr << "[*] DefineTaintSource : thread_id - " << thread_id << ", addr - " << addr << ", size - 0x" << hex << size << ", offset - 0x" << hex << offset << endl;

	Taint taint(addr, size, offset);

	DoTaintMemory(thread_id, addr, size, taint);

	m_taint_started = TRUE;
}