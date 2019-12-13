#ifndef _ARGUMENT_H_
#define _ARGUMENT_H_
#include "defines.h"

class Argument
{
	// 부모 클래스
};

class CreateFileArgument : public Argument
{
	// 저장할 데이터가 없음
};

class CreateFileMappingArgument : public Argument
{
	// 저장할 데이터가 없음
};

class ReadFileArgument : public Argument
{
protected:
	HANDLE		m_hFile;
	LPVOID		m_lpBuffer;
	DWORD		m_nNumberOfBytesToRead;
	LPDWORD		m_lpNumberOfBytesToRead;

public:
	ReadFileArgument(HANDLE			hFile,
					 LPVOID			lpBuffer,
					 DWORD			nNumberOfBytesToRead,
					 LPDWORD		lpNumberOfBytesToRead)
	{
		m_hFile					= hFile;
		m_lpBuffer				= lpBuffer;
		m_nNumberOfBytesToRead	= nNumberOfBytesToRead;
		m_lpNumberOfBytesToRead	= lpNumberOfBytesToRead;
	}

	HANDLE hFile()
	{
		return m_hFile;
	}

	LPVOID lpBuffer()
	{
		return m_lpBuffer;
	}

	LPDWORD lpNumberOfBytesToRead()
	{
		return m_lpNumberOfBytesToRead;
	}

	DWORD nNumberOfBytesToRead()
	{
		return m_nNumberOfBytesToRead;
	}
};

class ZwMapViewOfSectionArgument : public Argument
{
protected:
	PVOID*			m_BaseAddress;
	PLARGE_INTEGER	m_SectionOffset;
	PSIZE_T			m_ViewSize;

public:
	ZwMapViewOfSectionArgument( PVOID*			BaseAddress,
								PLARGE_INTEGER	SectionOffset,
								PSIZE_T			ViewSize)
	{
		m_BaseAddress			= BaseAddress;
		m_SectionOffset			= SectionOffset;
		m_ViewSize				= ViewSize;
	}

	PLARGE_INTEGER SectionOffset()
	{
		return m_SectionOffset;
	}

	PSIZE_T ViewSize()
	{
		return m_ViewSize;
	}

	PVOID* BaseAddress()
	{
		return m_BaseAddress;
	}
};

#endif