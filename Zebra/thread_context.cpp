#include "thread_context.h"	

VOID ThreadContext::SaveArgument(FUNCTION_TYPE type, Argument* arg)
{
	m_arg_storage.Save(type, arg);
}

Argument* ThreadContext::ReadArgument(FUNCTION_TYPE type)
{
	return m_arg_storage.Read(type);
}

VOID ThreadContext::DeleteArgument(FUNCTION_TYPE type)
{
	m_arg_storage.Delete(type);
}

BOOL ThreadContext::ExistArgument(FUNCTION_TYPE type)
{
	return m_arg_storage.Exist(type);
}

VOID ThreadContext::SetStackBaseAddress(ADDRINT stack_base_addr)
{
	m_stack_base_addr = stack_base_addr & 0xFFFF0000;

	cerr << "[*] ThreadContext::SetStackBaseAddress : " << hex << m_stack_base_addr << endl;
}

ADDRINT ThreadContext::GetStackBaseAddress()
{
	return m_stack_base_addr;
}

VOID ThreadContext::DoTaintMemory(ADDRINT addr, UINT32 size, Taint& taint)
{
	m_taint_manager.DoTaintMemory(addr, size, taint);
}


VOID ThreadContext::DoTaintRegister(REG reg, Taint& taint)
{
	m_taint_manager.DoTaintRegister(reg, taint);
}

VOID ThreadContext::UnTaintMemory(ADDRINT addr, UINT32 size)
{
	m_taint_manager.UnTaintMemory(addr, size);
}

VOID ThreadContext::UnTaintRegister(REG reg)
{
	m_taint_manager.UnTaintRegister(reg);
}

BOOL ThreadContext::IsTaintedMemory(ADDRINT addr, UINT32 size, Taint& taint)
{
	return m_taint_manager.IsTaintedMemory(addr, size, taint);
}

BOOL ThreadContext::IsTaintedRegister(REG reg, Taint& taint)
{
	return m_taint_manager.IsTaintedRegister(reg, taint);
}

/**
 * 주어진 주소가 스택 주소인지 확인
 * @param		addr		확인할 주소
 * @return		스택 주소 여부 
 */
BOOL ThreadContext::IsStackAddress(ADDRINT addr)
{
	BOOL result = FALSE;
	if (((UINT32)addr & 0xFFF00000) == m_stack_base_addr)
		result = TRUE;
	else
		result = FALSE;

	//cerr << "[*] ThreadContext::IsStackAddress : addr - " << hex << addr << ", result - " << (result ? "TRUE" : "FALSE") << endl;
	return result;
}

VOID ThreadContext::Dump()
{
	m_taint_manager.Dump();
}