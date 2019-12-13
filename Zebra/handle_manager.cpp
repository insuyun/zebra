#include "handle_manager.h"

/**
 * 생성자 
 */
HandleManager::HandleManager()
{
}

/**
 * 소멸자
 */
HandleManager::~HandleManager()
{

}

/**
 * 해당 핸들 등록
 * @param		handle		등록할 핸들
 */
VOID HandleManager::SaveHandle(HANDLE handle)
{
	m_handles.Save(handle);
}

/**
 * 핸들 삭제 
 * @param		handle		삭제할 핸들
 */
VOID HandleManager::DeleteHandle(HANDLE handle)
{
	m_handles.Delete(handle);
}

/**
 * 핸들의 존재 여부를 판단
 * @param		handle		체크할 핸들
 * @return		핸들의 존재 여부
 */
BOOL HandleManager::ExistHandle(HANDLE handle)
{
	return m_handles.Exist(handle);
}

// SaveHandle과 상동
VOID HandleManager::SaveMappingHandle(HANDLE handle)
{
	m_mapping_handles.Save(handle);
}

// DeleteHandle과 상동
VOID HandleManager::DeleteMappingHandle(HANDLE handle)
{
	m_mapping_handles.Delete(handle);
}

// ExistHandle과 상동
BOOL HandleManager::ExistMappingHandle(HANDLE handle)
{
	return m_mapping_handles.Exist(handle);
}