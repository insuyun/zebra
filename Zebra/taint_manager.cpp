#include "taint_manager.h"
#include <iostream>
#include <fstream>

#include <assert.h>

TaintManager::TaintManager()
{
}

TaintManager::~TaintManager()
{

}

UINT32 TaintManager::RegToInternalIndex(REG reg)
{

	// 레지스터에 맞게 인덱스를 리턴한다.

	REG container = REG_FullRegName(reg);
	UINT32 index;

	//cerr << "[*] RegToInternalIndex : " << REG_StringShort(reg) << endl;

	switch (container)
	{
	case REG_EAX: index = 0; break;
	case REG_EBX: index = 1; break;
	case REG_ECX: index = 2; break;
	case REG_EDX: index = 3; break;
	case REG_ESI: index = 4; break;
	case REG_EDI: index = 5; break;
	case REG_ESP: index = 6; break;
	case REG_EBP: index = 6; break;
	default:
		index = -1;
	}
	
	return index;
}

UINT32 TaintManager::RegAccessSize(REG reg)
{
	if (REG_is_Lower8(reg) || REG_is_Upper8(reg)) return 1;
	if (REG_is_Half16(reg)) return 2;
	if (REG_is_Half32(reg)) return 4;

	return 8;
}

VOID TaintManager::DoTaintRegister(REG reg, Taint& taint)
{
	UINT32 index = RegToInternalIndex(reg);

	taint.size = RegAccessSize(reg);

	m_tainted_registers[index] = taint;
}

VOID TaintManager::UnTaintRegister(REG reg)
{
	UINT32 index = RegToInternalIndex(reg);

	if (index == -1)
		return;

	m_tainted_registers[index].Invalidate();
}

BOOL TaintManager::IsTaintedRegister(REG reg, Taint& taint)
{
	UINT32 index = RegToInternalIndex(reg);

	if (index == -1)
		return FALSE;

	if (m_tainted_registers[index].IsValid())
	{
		taint = m_tainted_registers[index];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID TaintManager::DoTaintMemory(ADDRINT addr, UINT32 size, Taint& taint)
{

	cerr << "[*] ThreadContext::DoTaintMemory : addr - " << hex << addr << ", size - " << hex << size << endl;

	// 기존 테인트 제거
	UnTaintMemory(addr, size);

	taint.addr = addr;
	taint.size = size;

	m_tainted_memories.push_back(taint);
}

VOID TaintManager::UnTaintMemory(ADDRINT addr, UINT32 size)
{
	vector<Taint> additional_taints;
	vector<Taint>::iterator it;
	Taint taint;

	for (it = m_tainted_memories.begin(); it != m_tainted_memories.end(); it++)
	{
		ADDRINT low_addr = (*it).addr;
		ADDRINT high_addr = (*it).addr + (*it).size;

		// 테인트 메모리의 시작이 기존 테인트 사이에 존재할 경우
		if ( low_addr <= addr && addr < high_addr )
		{
			// 현재 테인트의 앞부분을 자른다
			(*it).size = addr - low_addr;

			// 뒷부분을 새로운 테인트를 만들어 벡터에 저장한 후 테인트 심볼로 추가한다
			if (addr + size < high_addr)
			{
				taint.Copy(addr +size, high_addr - (addr + size), *it);

				additional_taints.push_back(taint);
			}

		}

		// 테인트 메모리의 끝이 기존 테인트 사이에 존재할 경우
		// 시작점은 테인트 내부에 존재할 수 없으므로 남은 케이스는 시작점이 기존 테인트 시작점보다 작은 경우다
		else if ( low_addr <= addr + size  
					&& addr + size < high_addr)
		{
			(*it).size = high_addr - (addr + size);
			(*it).addr = addr + size;
		}
	}

	for (it = additional_taints.begin(); it != additional_taints.end(); it++)
	{
		m_tainted_memories.push_back(*it);
	}
}

BOOL TaintManager::IsTaintedMemory(ADDRINT addr, UINT32 size, Taint& taint)
{
	//Dump();

	BOOL result = FALSE;

	vector<Taint>::iterator it;

	for (it = m_tainted_memories.begin(); it != m_tainted_memories.end(); it++)
	{
		ADDRINT low_addr = (*it).addr;
		ADDRINT high_addr = (*it).addr + (*it).size;

		if ( low_addr <= addr && addr < high_addr )
		{
			taint.Copy(addr, size, *it);

			result = TRUE;
			break;
		}
	}

	if (result)
	{
		cerr << "[*] TaintManager::IsTaintedMemory : addr - " << hex << addr << ", size - " << size << endl;
	}
	
	return result;
}

VOID TaintManager::Dump()
{
	vector<Taint>::iterator it;

	cerr << "[*] Dump ----------------------------------------------" << endl;

	for (it = m_tainted_memories.begin(); it != m_tainted_memories.end(); it++)
	{
		Taint& taint = *it;

		cerr << "[**] [" << hex << taint.addr << ", " << hex << taint.size << "] : " << taint.symbol << endl;
	}

	cerr << "[*] ---------------------------------------------------" << endl;
}