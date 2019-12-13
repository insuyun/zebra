#ifndef _HANDLE_MANAGER_H_
#define _HANDLE_MANAGER_H_

#include <pin.H>
#include <vector>

#include "defines.h"
#include "lockable_handle_container.h"

class HandleManager
{
protected:
	LockableHandleContainer m_handles;
	LockableHandleContainer m_mapping_handles;
	PIN_LOCK m_lock;

public:
	HandleManager();
	~HandleManager();

	// 일반 핸들 관련 함수
	VOID SaveHandle(HANDLE handle);
	VOID DeleteHandle(HANDLE handle);
	BOOL ExistHandle(HANDLE handle);

	// 매핑 핸들 관련 함수
	VOID SaveMappingHandle(HANDLE handle);
	VOID DeleteMappingHandle(HANDLE handle);
	BOOL ExistMappingHandle(HANDLE handle);
};


#endif 