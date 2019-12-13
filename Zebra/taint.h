#ifndef _TAINT_H_
#define _TAINT_H_

#include "defines.h"

class Taint
{
public:
	static const int INVALID = 0xffffffff;
	
	ADDRINT addr;
	UINT32 size;
	UINT32 offset;		// 소스의 오프셋
	string symbol;

	Taint::Taint()
	{
		addr = INVALID;
		size = INVALID;
		offset = INVALID;
		symbol = "";
		
	}

	Taint::Taint(ADDRINT addr, UINT32 size)
	{
		this->addr = addr;
		this->size = size;
		this->offset = INVALID;
		symbol = "";
	}

	Taint::Taint(ADDRINT addr, UINT32 size, UINT32 offset)
	{
		this->addr = addr;
		this->size = size;
		this->offset = offset;
		symbol = "";
	}

	Taint::Taint(ADDRINT addr, UINT32 size, string symbol)
	{
		this->addr = addr;
		this->size = size;
		this->offset = INVALID;
		this->symbol = symbol;
	}

	VOID Taint::Invalidate()
	{
		addr = INVALID;
		size = INVALID;
		offset = INVALID;
		symbol = "";
	}

	BOOL Taint::IsValid()
	{
		return (offset != INVALID) || (symbol != "");
	}

	BOOL Taint::FromSource()
	{
		return offset != INVALID;
	}

	ADDRINT high_addr()
	{
		return addr + size;
	}

	VOID Dump()
	{
		cerr << "[*] Taint::Dump" << endl;
		cerr << "[**] addr : " << hex << addr << endl;
		cerr << "[**] size : " << hex << size << endl;
		cerr << "[**] offset : " << hex << offset << endl;
		cerr << "[**] symbol : " << hex << symbol << endl;
	}

	/**
	 * 소스를 바탕으로 결과 테인트를 세팅한다.
	 * 주소와 사이즈는 주어진 값으로 오프셋과 심볼은 소스 테인트를 참조한다
	 * @param		addr		테인트 주소
	 * @param		size		테인트 사이즈
	 * @param		dest		결과 테인트
	 * @param		src			소스 테인트
	 */
	VOID Copy(ADDRINT addr, UINT32 size, Taint& src)
	{
		this->addr = addr;
		this->size = size;
		this->symbol = src.symbol;

		if (src.FromSource())
		{
			this->offset = src.offset + addr - src.addr;
		}
	}
};

#endif // _TAINT_H_