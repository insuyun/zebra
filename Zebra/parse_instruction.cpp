#include "parse_instruction.h"

extern ProcessContext g_process_context;

Taint GenerateTaint(SYMBOLIC_TYPE sym_type, string symbol, 
						Taint& src_taint,
						ADDRINT src_val, 
						Taint& dest_taint,
						ADDRINT dest_val)
{
	Taint result_taint;

	//
	//          src     dest
	//   taint   x        x      fuck no
	//           x        o      sym(out) = sym(dest) + operator + actual(src)  = [dest +(src)] symbolic update
	//           o        x      sym(out) = sym(src)  + operator + actual(dest) = [dest <- src] taint propagation
	//           o        o      sym(out) = sym(dest) + operator + sym(src)     = [dest  + src] symbolic combination
	//


	if (symbol != "") // if operator exists (if it isnt a DATAXFER class)
	{
		string src_symbol = src_taint.symbol;
		string dest_symbol = dest_taint.symbol;

		if (src_taint.FromSource()) 
			src_symbol = "File[" + decstr(src_taint.offset) + ":" + decstr(src_taint.offset + src_taint.size) + "]";

		if (dest_taint.FromSource())
			dest_symbol = "File[" + decstr(dest_taint.offset) + ":" + decstr(dest_taint.offset + dest_taint.size) + "]";

#if 0
		// 디버깅용 
		cerr << "[*] GenerateTaint : sym_type - " << sym_type << ", symbol - \"" << symbol <<  "\", src_symbol - " << src_symbol 
			 << ", dest_symbol - " << dest_symbol << ", src_val - " << src_val << ", dest_val - " << dest_val << endl;

		cerr << "[**] src_taint" << endl;
		src_taint.Dump();

		cerr << "[**] dest_taint" << endl;
		dest_taint.Dump();
		// 디버깅용
#endif
		if (src_taint.IsValid() && dest_taint.IsValid()) // case 3
		{
			//cerr << "[**] Type 1" << endl;
			switch (sym_type)
			{
				case SYMBOLIC_PREFIX: 
					src_symbol = symbol + "(" + dest_symbol + ", " + src_symbol + ")"; 
					break;

				case SYMBOLIC_INFIX:  
					src_symbol = "(" + src_symbol + symbol + dest_symbol + ")";
					break;

				case SYMBOLIC_POSTFIX:
					src_symbol = "(" + dest_symbol + ", " + src_symbol + ")" + symbol; 
					break;
				default: 
					break;
			}
		}
		else if (src_taint.IsValid() && !dest_taint.IsValid()) // case 2
		{
			//cerr << "[**] Type 2" << endl;
			switch (sym_type)
			{
			case SYMBOLIC_PREFIX: 
				src_symbol = symbol + "(" + hexstr(dest_val) + ", " + src_symbol + ")"; 
				break;

			case SYMBOLIC_INFIX:  
				src_symbol = "(" + src_symbol + symbol + hexstr(dest_val) + ")"; 
				break;

			case SYMBOLIC_POSTFIX:
				src_symbol = "(" + hexstr(dest_val) + ", " + src_symbol + ")" + symbol; break;
			default: break;
			}
		}

		else if (!src_taint.IsValid() && dest_taint.IsValid()) // case 1
		{
			//cerr << "[**] Type 3" << endl;
			switch (sym_type)
			{
			case SYMBOLIC_PREFIX: 
				src_symbol = symbol + "(" + dest_symbol + ", " + hexstr(src_val) + ")"; 
				break;

			case SYMBOLIC_INFIX:  
				src_symbol = "(" + dest_symbol  + symbol + hexstr(src_val) + ")"; 
				break;

			case SYMBOLIC_POSTFIX:
				src_symbol = "(" + dest_symbol + ", " + hexstr(src_val) + ")" + symbol; 
				break;

			default: 
				break;
			}
		}

		result_taint.symbol = src_symbol;
		//cerr << "[**] src_symbol : " << src_symbol << endl;
	}

//	cerr << "[**] result_taint " << endl;
//	result_taint.Dump();

	return result_taint;
}


VOID ParseMovRegReg(THREADID thread_id,
					VOID* instruction_ptr, 
					REG dest_reg,
					REG src_reg)
{
	//cerr << "[*] ParseMovRegReg : " << hex << instruction_ptr << endl;

	Taint dest_taint, src_taint;
	BOOL dest_tainted = g_process_context.IsTaintedRegister(thread_id, dest_reg, dest_taint);
	BOOL src_tainted = g_process_context.IsTaintedRegister(thread_id, src_reg, src_taint);

	if (src_tainted)
	{
		g_process_context.DoTaintRegister(thread_id, dest_reg, src_taint);
	}
	else if (dest_tainted)
	{
		g_process_context.UnTaintRegister(thread_id, dest_reg);
	}
}

VOID ParseMovRegMem(THREADID thread_id,
					VOID* instruction_ptr,
					REG dest_reg,
					ADDRINT src_addr,
					UINT32 src_size)
{
	//cerr << "[*] ParseMovRegMem : " << hex << instruction_ptr << endl;

	Taint dest_taint, src_taint;
	BOOL dest_tainted = g_process_context.IsTaintedRegister(thread_id, dest_reg, dest_taint);
	BOOL src_tainted = g_process_context.IsTaintedMemory(thread_id, src_addr, src_size, src_taint);

	if (src_tainted)
	{
		g_process_context.DoTaintRegister(thread_id, dest_reg, src_taint);
	}
	else if (dest_tainted)
	{
		g_process_context.UnTaintRegister(thread_id, dest_reg);
	}
}

VOID ParseMovMemReg(THREADID thread_id,
					VOID* instruction_ptr,
					ADDRINT dest_addr,
					UINT32 dest_size,
					REG src_reg)
{
	//cerr << "[*] ParseMovMemReg : " << hex << instruction_ptr << endl;

	Taint dest_taint, src_taint;
	BOOL dest_tainted = g_process_context.IsTaintedMemory(thread_id, dest_addr, dest_size, dest_taint);
	BOOL src_tainted = g_process_context.IsTaintedRegister(thread_id, src_reg, src_taint);

	if (src_tainted)
	{
		g_process_context.DoTaintMemory(thread_id, dest_addr, dest_size, src_taint);
	}
	else if (dest_tainted)
	{
		g_process_context.UnTaintMemory(thread_id, dest_addr, dest_size);
	}
}

VOID ParseMovMemMem(THREADID thread_id,
					VOID* instruction_ptr,
					ADDRINT dest_addr,
					UINT32 dest_size,
					ADDRINT src_addr,
					UINT32 src_size)
{
	//cerr << "[*] ParseMovMemMem : " << hex << instruction_ptr << endl;

	Taint dest_taint, src_taint;
	BOOL dest_tainted = g_process_context.IsTaintedMemory(thread_id, dest_addr, dest_size, dest_taint);
	BOOL src_tainted = g_process_context.IsTaintedMemory(thread_id, src_addr, src_size, src_taint);

	if (src_tainted)
	{
		g_process_context.DoTaintMemory(thread_id, dest_addr, dest_size, src_taint);
	}
	else if (dest_tainted)
	{
		g_process_context.UnTaintMemory(thread_id, dest_addr, dest_size);
	}
}

VOID ParseBinaryOperatorRegReg(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol, 
								REG dest_reg,
								UINT32 dest_val,
								REG src_reg,
								UINT32 src_val)
{
	//cerr << "[*] ParseBinaryOperatorRegReg : " << hex << instruction_ptr << endl;

	Taint src_taint, dest_taint;
	BOOL src_tainted = g_process_context.IsTaintedRegister(thread_id, src_reg, src_taint);
	BOOL dest_tainted = g_process_context.IsTaintedRegister(thread_id, dest_reg, dest_taint);


	// 소스 테인트 
	if (src_tainted)
	{
		Taint& taint = GenerateTaint(sym_type, symbol, src_taint, src_val, dest_taint, dest_val);

		g_process_context.DoTaintRegister(thread_id, dest_reg, taint);

	} // 목적지 테인트
	else if (dest_tainted)
	{
		g_process_context.UnTaintRegister(thread_id, dest_reg);
	}
}

VOID ParseBinaryOperatorRegMem(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol, 
								REG dest_reg,
								UINT32 dest_val,
								ADDRINT src_addr,
								UINT32 src_size)
{
	//cerr << "[*] ParseBinaryOperatorRegMem : " << hex << instruction_ptr << endl;

	Taint src_taint, dest_taint;
	BOOL src_tainted = g_process_context.IsTaintedMemory(thread_id, src_addr, src_size, src_taint);
	BOOL dest_tainted = g_process_context.IsTaintedRegister(thread_id, dest_reg, dest_taint);

	// 소스 테인트 
	if (src_tainted)
	{
		ADDRINT src_val = 0;
		
		PIN_SafeCopy(&src_val, (VOID*)src_addr, src_size);

		Taint& taint = GenerateTaint(sym_type, symbol, src_taint, src_val, dest_taint, dest_val);

		g_process_context.DoTaintRegister(thread_id, dest_reg, taint);

	} // 목적지 테인트
	else if (dest_tainted)
	{
		g_process_context.UnTaintRegister(thread_id, dest_reg);
	}
}

VOID ParseBinaryOperatorMemReg(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol, 
								ADDRINT dest_addr,
								UINT32 dest_size,
								REG src_reg,
								UINT32 src_val)
{
	//cerr << "[*] ParseBinaryOperatorMemReg : " << hex << instruction_ptr << endl;

	Taint src_taint, dest_taint;
	BOOL src_tainted = g_process_context.IsTaintedRegister(thread_id, src_reg, src_taint);
	BOOL dest_tainted = g_process_context.IsTaintedMemory(thread_id, dest_addr, dest_size, dest_taint);

	// 소스 테인트 
	if (src_tainted)
	{
		ADDRINT dest_val = 0;
		
		PIN_SafeCopy(&dest_val, (VOID*)dest_addr, dest_size);

		Taint& taint = GenerateTaint(sym_type, symbol, src_taint, src_val,  dest_taint, dest_val);

		g_process_context.DoTaintMemory(thread_id, dest_addr, dest_size, taint);

	} // 목적지 테인트
	else if (dest_tainted)
	{
		g_process_context.UnTaintMemory(thread_id, dest_addr, dest_size);
	}
}

VOID ParseBinaryOperatorRegImm(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol,
								REG dest_reg,
								ADDRINT imm)
{
	Taint dest_taint, src_taint;
	BOOL dest_tainted = g_process_context.IsTaintedRegister(thread_id, dest_reg, dest_taint);

	if (dest_tainted)
	{
		// dest가 taint되어 있으므로 dest_val 값은 더미값으로 imm을 입력
		Taint& taint = GenerateTaint(sym_type, symbol, src_taint, imm, dest_taint, 0xdeadbeef);

		g_process_context.DoTaintRegister(thread_id, dest_reg, taint);

	}
	else
		g_process_context.UnTaintRegister(thread_id, dest_reg);
}

VOID ParseBinaryOperatorMemImm(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol,
								ADDRINT dest_addr,
								UINT32 dest_size,
								ADDRINT imm)
{
	Taint dest_taint, src_taint;
	BOOL dest_tainted = g_process_context.IsTaintedMemory(thread_id, dest_addr, dest_size, dest_taint);

	if (dest_tainted)
	{
		Taint& taint = GenerateTaint(sym_type, symbol, src_taint, imm, dest_taint, 0xdeadbeef);

		g_process_context.DoTaintMemory(thread_id, dest_addr, dest_size, taint);
	}
	else
		g_process_context.UnTaintMemory(thread_id, dest_addr, dest_size);
}

// MOVS
VOID ParseMovs(THREADID thread_id,
				VOID* instruction_ptr,
				ADDRINT dest_addr,
				ADDRINT src_addr,
				UINT32 bytes)
{
	ParseMovMemMem(thread_id, instruction_ptr, dest_addr,  bytes, src_addr, bytes);
}

// 초기화

VOID ParseFlushRegister(THREADID thread_id, 
						VOID* instruction_ptr,
						REG reg)

{
	//cerr << "[*] ParseFlush : " << hex << instruction_ptr << endl;

	g_process_context.UnTaintRegister(thread_id, reg);
}

VOID ParseFlushMemory(THREADID thread_id,
						VOID* instruction_ptr,
						ADDRINT addr,
						UINT32 size)
{
	g_process_context.UnTaintMemory(thread_id, addr, size);
}

// PUSH
VOID ParsePushReg(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt, 
					REG reg)
{
	// PUSH 시에는 현재 ESP 보다 4 낮은 값에 데이터가 기록됨
	UINT32 dest_size	= 4;
	ADDRINT dest_addr	= PIN_GetContextReg(ctxt, REG_ESP) - dest_size;

	ParseMovMemReg(thread_id, instruction_ptr, dest_addr, dest_size, reg);
}

VOID ParsePushMem(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt, 
					ADDRINT addr,
					UINT32 size)
{
	// PUSH 시에는 현재 ESP 보다 4 낮은 값에 데이터가 기록됨
	UINT32 dest_size	= 4;
	ADDRINT dest_addr	= PIN_GetContextReg(ctxt, REG_ESP) - dest_size;

	ParseMovMemMem(thread_id, instruction_ptr, dest_addr, dest_size, addr, size);	
}

VOID ParsePushImm(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt)
{
	// PUSH 시에는 현재 ESP 보다 4 낮은 값에 데이터가 기록됨
	UINT32 dest_size	= 4;
	ADDRINT dest_addr	= PIN_GetContextReg(ctxt, REG_ESP) - dest_size;

	ParseFlushMemory(thread_id, instruction_ptr, dest_addr, dest_size);
}

// POP
VOID ParsePopReg(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt,
					REG reg)
{
	UINT32 src_size		= 4;
	ADDRINT src_addr	= PIN_GetContextReg(ctxt, REG_ESP);

	ParseMovRegMem(thread_id, instruction_ptr, reg, src_addr, src_size);
}

VOID ParsePopMem(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt,
					ADDRINT addr,
					UINT32 size)
{
	UINT32 src_size	= 4;
	ADDRINT src_addr	= PIN_GetContextReg(ctxt, REG_ESP);

	ParseMovMemMem(thread_id, instruction_ptr, addr, size, src_addr, src_size);
}

VOID ParseLeaReg(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt,
					REG dest, 
					REG base, 
					REG index, 
					INT32 displacement,
					UINT32 scale)
{
	Taint dest_taint, base_taint, index_taint, dummy;

	BOOL base_tainted	= FALSE;
	BOOL index_tainted	= FALSE;

	BOOL base_valid		= FALSE;
	BOOL index_valid	= FALSE;

	UINT32 base_val		= 0;
	UINT32 index_val	= 0;

	// 유효성 검증 
	if (REG_valid(base))
	{
		base_valid		= TRUE;
		base_tainted	= g_process_context.IsTaintedRegister(thread_id, base, base_taint);
		base_val		= PIN_GetContextReg(ctxt, base);
	}

	if (REG_valid(index))
	{
		index_valid		= TRUE;
		index_tainted	= g_process_context.IsTaintedRegister(thread_id, index, index_taint);
		index_val		= PIN_GetContextReg(ctxt, index);
	}

	// 테인트 시작
	if (base_tainted == FALSE &&
		index_tainted == FALSE)
	{
		// 둘 다 테인트가 안되어 있을 경우에는 dest의 테인트가 사라진다.
		g_process_context.UnTaintRegister(thread_id, dest);
		return;
	}
	else if (base_tainted == TRUE &&
				index_tainted == FALSE)
	{
		dest_taint = base_taint;

		if (index_valid)
			displacement += scale * index_val;
	}

	else if (base_tainted == FALSE &&
				index_tainted == TRUE)
	{
		if (scale == 1)
			dest_taint = index_taint;
		else
			dest_taint = GenerateTaint(SYMBOLIC_INFIX, "*", index_taint, 0xdeadbeef, dummy, scale);

		if (base_valid == TRUE)
			displacement += base_val;

	}
	else
	{
		if (scale != 1)
			index_taint = GenerateTaint(SYMBOLIC_INFIX, "*", index_taint, 0xdeadbeef, dummy, scale);

		if (!index_taint.IsValid())
		{
			cerr << "[*] ParseLeaReg : ERROR" << endl;
		}

		dest_taint = GenerateTaint(SYMBOLIC_INFIX, "+", base_taint, 0xdeadbeef, index_taint, 0xbadf00d);
	}

	if (displacement > 0)
		dest_taint = GenerateTaint(SYMBOLIC_INFIX, "+", dest_taint, 0xdeadbeef, dummy, displacement);
	else if (displacement < 0)
		dest_taint = GenerateTaint(SYMBOLIC_INFIX, "-", dest_taint, 0xdeadbeef, dummy, displacement);

	g_process_context.DoTaintRegister(thread_id, dest, dest_taint);

}

VOID ParseRet(THREADID thread_id,
				VOID* instruction_ptr,
				CONTEXT* ctxt)
{
	ADDRINT esp = PIN_GetContextReg(ctxt, REG_ESP);

	g_process_context.UnTaintMemory(thread_id, esp - 0x10000, 0x10000 + sizeof(ADDRINT));
}