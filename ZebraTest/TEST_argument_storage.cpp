#include <gtest/gtest.h>
#include <Zebra/argument_storage.h>

// ReadFile의 인자가 제대로 저장되는지 확인
TEST(ArgumentStorage, ReadFileArgument)
{
	// 테스트 데이터
	HANDLE			hFile = (LPVOID)0x01234567;
	LPVOID			lpBuffer = (LPVOID)0x12345678;
	DWORD			nNumberOfBytesToRead = (DWORD)0x23456789;
	LPDWORD			lpNumberOfBytesToRead = (LPDWORD)0x34567890;

	ArgumentStorage storage;
	ReadFileArgument*		arg = new ReadFileArgument(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesToRead);
	ReadFileArgument*		tmp;

	ASSERT_FALSE(storage.Exist(READ_FILE));
	
	storage.Save(READ_FILE, arg);

	ASSERT_TRUE(storage.Exist(READ_FILE));

	tmp = (ReadFileArgument*)storage.Read(READ_FILE);

	ASSERT_TRUE(tmp->lpBuffer() == lpBuffer);
	ASSERT_TRUE(tmp->nNumberOfBytesToRead() == nNumberOfBytesToRead);
	ASSERT_TRUE(tmp->lpNumberOfBytesToRead() == lpNumberOfBytesToRead);

	storage.Delete(READ_FILE);

	ASSERT_FALSE(storage.Exist(READ_FILE));
}

// ZwMapViewOfSection의 인자가 제대로 저장되는지 확인
TEST(ArgumentStorage, ZwMapViewOfSectionArgument)
{
	// 테스트 데이터
	PVOID*			BaseAddress = (PVOID*)0x12345678;
	PLARGE_INTEGER	SectionOffset = (PLARGE_INTEGER)0x23456789;
	PSIZE_T			ViewSize = (PSIZE_T)0x34567890;

	ArgumentStorage storage;
	ZwMapViewOfSectionArgument*		arg = new ZwMapViewOfSectionArgument(BaseAddress, SectionOffset, ViewSize);
	ZwMapViewOfSectionArgument*		tmp;

	ASSERT_FALSE(storage.Exist(ZW_MAP_VIEW_OF_SECTION));
	
	storage.Save(ZW_MAP_VIEW_OF_SECTION, arg);

	ASSERT_TRUE(storage.Exist(ZW_MAP_VIEW_OF_SECTION));

	tmp = (ZwMapViewOfSectionArgument*)storage.Read(ZW_MAP_VIEW_OF_SECTION);

	ASSERT_TRUE(tmp->BaseAddress() == BaseAddress);
	ASSERT_TRUE(tmp->SectionOffset() == SectionOffset);
	ASSERT_TRUE(tmp->ViewSize() == ViewSize);

	storage.Delete(ZW_MAP_VIEW_OF_SECTION);

	ASSERT_FALSE(storage.Exist(ZW_MAP_VIEW_OF_SECTION));
}