#ifndef _LOCKABLE_HANDLE_CONTAINER_H_
#define _LOCKABLE_HANDLE_CONTAINER_H_

#include "defines.h"

class LockableHandleContainer
{
private:
	PIN_LOCK		m_lock;
	vector<HANDLE>	m_handles;

public:
	LockableHandleContainer();

	VOID Save(HANDLE handle);
	BOOL Exist(HANDLE handle);
	VOID Delete(HANDLE handle);

};
#endif // _LOCKABLE_HANDLE_CONTAINER_H_