#include "lockable_handle_container.h"	

LockableHandleContainer::LockableHandleContainer()
{
	PIN_InitLock(&m_lock);
}

VOID LockableHandleContainer::Save(HANDLE handle)
{
	PIN_GetLock(&m_lock, PIN_ThreadId());

	m_handles.push_back(handle);

	PIN_ReleaseLock(&m_lock);
}

BOOL LockableHandleContainer::Exist(HANDLE handle)
{
	vector<HANDLE>::iterator it;
	BOOL result = FALSE;

	PIN_GetLock(&m_lock, PIN_ThreadId());

	for (it = m_handles.begin(); it != m_handles.end(); it++)
	{
		if ((*it) == handle)
		{
			result = TRUE;
			break;
		}
	}

	PIN_ReleaseLock(&m_lock);

	return result;
}

VOID LockableHandleContainer::Delete(HANDLE handle)
{
	vector<HANDLE>::iterator it;

	PIN_GetLock(&m_lock, PIN_ThreadId());

	for (it = m_handles.begin(); it != m_handles.end(); it++)
	{
		if ((*it) == handle)
		{
			m_handles.erase(it);
			break;
		}
	}

	PIN_ReleaseLock(&m_lock);
}