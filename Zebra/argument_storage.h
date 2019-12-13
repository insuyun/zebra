#ifndef _ARGUMENT_STORAGE_H_
#define _ARGUMENT_STORAGE_H_

#include "argument.h"

enum FUNCTION_TYPE
{
	CREATE_FILE,
	CREATE_FILE_MAPPING,
	READ_FILE,
	ZW_MAP_VIEW_OF_SECTION,
	FUNCTION_TYPE_MAX,
};

// 함수의 인자 저장소 
class ArgumentStorage
{
protected:
	Argument*	m_arg[FUNCTION_TYPE_MAX];

public:
	ArgumentStorage();

	~ArgumentStorage();

	BOOL		Exist(FUNCTION_TYPE func_type);

	VOID		Delete(FUNCTION_TYPE func_type);

	Argument*	Read(FUNCTION_TYPE func_type);

	VOID		Save(FUNCTION_TYPE func_type, Argument* arg);
};

#endif /*_ARGUMENT_STORAGE_H_ */