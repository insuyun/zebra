#include "argument_storage.h"

#include <assert.h>
#include <stdio.h>

// 생성자
ArgumentStorage::ArgumentStorage()
{
	memset(m_arg, 0, sizeof(m_arg));
}

// 소멸자
ArgumentStorage::~ArgumentStorage()
{
	int i = 0;

	for (i = 0 ; i < FUNCTION_TYPE_MAX; i++)
	{
		if (m_arg[i] != NULL)
			delete m_arg[i];
	}
}

/**
 * 해당 함수의 인자의 저장 여부를 확인
 * @param	func_type	함수의 타입
 * @return	/DTARGET_IA32 		함수의 인자가 저장되어있으면 TRUE, 아닐 경우 FALSE
 */
BOOL ArgumentStorage::Exist(FUNCTION_TYPE func_type)
{
	BOOL result = (m_arg[func_type] != NULL);

	return result;
}

/**
 * 해당 함수의 인자를 삭제
 * @param	func_type	함수의 타입
 */
VOID ArgumentStorage::Delete(FUNCTION_TYPE func_type)
{
	if (m_arg[func_type] != NULL)
	{
		delete m_arg[func_type];
		m_arg[func_type] = NULL;
	}
}

/**
 * 해당 함수의 인자를 저장
 * @param	func_type	함수의 타입
 * @param	arg			함수의 인자
 */
VOID ArgumentStorage::Save(FUNCTION_TYPE func_type, Argument* arg)
{
	if (m_arg[func_type] != NULL)
		Delete(func_type);

	m_arg[func_type] = arg;
}

/**
 * 해당 함수의 인자를 저장
 * @param	func_type	함수의 타입
 * @return	Argument*	함수의 인자 
 */
Argument* ArgumentStorage::Read(FUNCTION_TYPE func_type)
{
	return m_arg[func_type];
}