#include "defines.h"

VOID PreCreateFileW(THREADID thread_id, 
					LPCWSTR file_name);

VOID PostCreateFileW(THREADID thread_id, 
						HANDLE return_value);

VOID PreReadFile(THREADID thread_id, 
					HANDLE handle, 
					LPVOID lpBuffer, 
					DWORD nNumberOfBytesToRead, 
					LPDWORD lpNumberOfBytesToRead);

VOID PostReadFile(THREADID thread_id);

VOID PreCreateFileMapping(THREADID thread_id, 
							HANDLE handle);

VOID PostCreateFileMapping(THREADID thread_id, 
							HANDLE return_value);

VOID PreZwMapViewOfSection(THREADID thread_id, 
							HANDLE mapping_handle,
							PVOID* base_address, 
							PLARGE_INTEGER section_offset, 
							PSIZE_T view_size);

VOID PostZwMapViewOfSection(THREADID thread_id);

VOID CheckArgumentTainted(THREADID thread_id,
							ADDRINT return_address,
							CONTEXT* ctxt,
							char* func_name,
							UINT32 arg_position);

VOID CheckRegisterTainted(THREADID thread_id,
						ADDRINT return_address,
						char* func_name,
						REG reg);