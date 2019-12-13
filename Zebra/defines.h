#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <pin.H>

#include <iostream>
#include <fstream>

// WINDOWS TYPE
typedef const wchar_t* LPCWSTR;
typedef void* HANDLE;
typedef void* LPVOID;
typedef void* PVOID;

typedef unsigned long DWORD;
typedef DWORD *LPDWORD;

typedef long LONG;
typedef __int64 LONGLONG;

typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR SIZE_T, *PSIZE_T;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG HighPart;
	};

	struct {
		DWORD LowPart;
		LONG HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

#define MAX_THREADS				100
#define USER_DEFINED_FILE		L"C:\\hacking\\sample.hwp"

#endif // _DEFINES_H_ 
