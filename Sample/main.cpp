#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#define FILE_NAME L"C:\\hacking\\sample.hwp"

void ThreadFunc(LPVOID arg)
{
	printf("[*] Thread created\n");
}

VOID CreateDummyThread()
{
	DWORD dwThreadId;

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, 0, &dwThreadId);
}

VOID CompoundReadFile(VOID* buf, DWORD dwNumberOfBytesToRead)
{
	IStorage* pStorage = NULL;

	StgOpenStorage(FILE_NAME, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStorage);

	IEnumSTATSTG *estg;

	pStorage->EnumElements(0, NULL, 0, &estg);

	STATSTG stgstruct = {0};

	unsigned long fetched = 0;

	while (S_OK == estg->Next(1, &stgstruct, &fetched))
	{
		if (STGTY_STREAM == stgstruct.type && !wcscmp(stgstruct.pwcsName, L"WorkBook"))
		{
			IStream* pStream = NULL;
			ULONG cbRead;

			pStorage->OpenStream(stgstruct.pwcsName, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
			pStream->Read(buf, dwNumberOfBytesToRead, &cbRead);
		}
	}
}

VOID SimpleReadFile(VOID* buf, DWORD dwNumberOfBytesToRead)
{
	HANDLE hFile = CreateFileW(FILE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	DWORD size;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFileW\n");
		exit(-1);
	}

	ReadFile(hFile, buf, dwNumberOfBytesToRead, &size, NULL);
}

void main(int argc, char** argv)
{
	char buf[0x1000];

	//CreateDummyThread();

	//SimpleReadFile(buf, 0x1000);
	CompoundReadFile(buf, 0x1000);

	printf("[*] Size : %x\n", buf[4] & 0xff);
	void *a = malloc(buf[4] & 255);
}
