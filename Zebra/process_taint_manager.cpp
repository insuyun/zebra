#include "process_taint_manager.h"

ProcessTaintManager::ProcessTaintManager()
{
	PIN_InitLock(&m_lock);
}

VOID ProcessTaintManager::DoTaintRegister(REG reg, Taint& taint)
{
	PIN_GetLock(&m_lock, PIN_ThreadId());
	
	m_taint_manager.DoTaintRegister(reg, taint);

	PIN_ReleaseLock(&m_lock);
}

VOID ProcessTaintManager::UnTaintRegister(REG reg)
{
	PIN_GetLock(&m_lock, PIN_ThreadId());

	m_taint_manager.UnTaintRegister(reg);

	PIN_ReleaseLock(&m_lock);
}

BOOL ProcessTaintManager::IsTaintedRegister(REG reg, Taint& taint)
{
	BOOL result = FALSE;

	PIN_GetLock(&m_lock, PIN_ThreadId());

	result = m_taint_manager.IsTaintedRegister(reg, taint);

	PIN_ReleaseLock(&m_lock);

	return result;
}

VOID ProcessTaintManager::DoTaintMemory(ADDRINT addr, UINT32 size, Taint& taint)
{
	PIN_GetLock(&m_lock, PIN_ThreadId());
	
	m_taint_manager.DoTaintMemory(addr, size, taint);

	PIN_ReleaseLock(&m_lock);
}

VOID ProcessTaintManager::UnTaintMemory(ADDRINT addr, UINT32 size)
{
	PIN_GetLock(&m_lock, PIN_ThreadId());

	m_taint_manager.UnTaintMemory(addr, size);

	PIN_ReleaseLock(&m_lock);
}

BOOL ProcessTaintManager::IsTaintedMemory(ADDRINT addr, UINT32 size, Taint& taint)
{
	BOOL result = FALSE;

	PIN_GetLock(&m_lock, PIN_ThreadId());

	result = m_taint_manager.IsTaintedMemory(addr, size, taint);

	PIN_ReleaseLock(&m_lock);

	return result;
}