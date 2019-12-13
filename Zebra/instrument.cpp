#include "instrument.h"
#include "function_callback.h"
#include "process_context.h"
#include "parse_instruction.h"
#include "instruction_utils.h"
#include "signature_finder.h"
#include "image_utils.h"

extern ProcessContext g_process_context;
SignatureFinder		g_signature_finder;

VOID Image(IMG img, VOID * v)
{
	//IMG_PrintInstruction(img, "HwpApp.dll", 0x0117445);

	for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym))
	{
		string undFuncName = PIN_UndecorateSymbolName(SYM_Name(sym), UNDECORATION_NAME_ONLY);

		//cerr << "[*] Image : " << undFuncName << endl;

		if (undFuncName == "CreateFileW")
		{
			RTN createRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

			if (RTN_Valid(createRtn))
			{
				RTN_Open(createRtn);
				RTN_InsertCall(createRtn, IPOINT_BEFORE, (AFUNPTR)PreCreateFileW,
					IARG_THREAD_ID,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
					IARG_END);

				RTN_InsertCall(createRtn, IPOINT_AFTER, (AFUNPTR)PostCreateFileW,
					IARG_THREAD_ID,
					IARG_FUNCRET_EXITPOINT_VALUE,
					IARG_END);

				RTN_Close(createRtn);
			}
		}
		else if (undFuncName == "ReadFile")
		{
			RTN readRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

			if (RTN_Valid(readRtn))
			{
				RTN_Open(readRtn);
				RTN_InsertCall(readRtn, IPOINT_BEFORE, (AFUNPTR)PreReadFile,
					IARG_THREAD_ID,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
					IARG_END);

				RTN_InsertCall(readRtn, IPOINT_AFTER, (AFUNPTR)PostReadFile,
					IARG_THREAD_ID,
					IARG_END);

				RTN_Close(readRtn);
			}
		}
		else if (undFuncName == "CreateFileMappingW")
		{
			RTN mappingRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

			if (RTN_Valid(mappingRtn))
			{
				RTN_Open(mappingRtn);
				RTN_InsertCall(mappingRtn, IPOINT_BEFORE, (AFUNPTR)PreCreateFileMapping,
					IARG_THREAD_ID,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
					IARG_END);

				RTN_InsertCall(mappingRtn, IPOINT_AFTER, (AFUNPTR)PostCreateFileMapping,
					IARG_THREAD_ID,
					IARG_FUNCRET_EXITPOINT_VALUE,
					IARG_END);
				RTN_Close(mappingRtn);
			}
		}

		else if (undFuncName == "ZwMapViewOfSection")
		{
			RTN mappingRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

			if (RTN_Valid(mappingRtn))
			{
				RTN_Open(mappingRtn);

				RTN_InsertCall(mappingRtn, IPOINT_BEFORE, (AFUNPTR)PreZwMapViewOfSection,
					IARG_THREAD_ID,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 5,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 6,
					IARG_END);

				RTN_InsertCall(mappingRtn, IPOINT_AFTER, (AFUNPTR)PostZwMapViewOfSection,
					IARG_THREAD_ID,
					IARG_END);

				RTN_Close(mappingRtn);
			}
		}

		else if (undFuncName == "malloc")
		{
			RTN mappingRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

			if (RTN_Valid(mappingRtn))
			{
				RTN_Open(mappingRtn);
				RTN_InsertCall(mappingRtn, IPOINT_BEFORE, (AFUNPTR)CheckArgumentTainted,
					IARG_THREAD_ID,
					IARG_RETURN_IP,
					IARG_CONTEXT,
					IARG_ADDRINT, "malloc",
					IARG_UINT32, 1,
					IARG_END);
				RTN_Close(mappingRtn);
			}
		}

		else if (undFuncName == "alloca_probe_16")
		{
			
			RTN mappingRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

			if (RTN_Valid(mappingRtn))
			{
				cerr << "[*] Address : " << hex << RTN_Address(mappingRtn) << endl;

				RTN_Open(mappingRtn);
				RTN_InsertCall(mappingRtn, IPOINT_BEFORE, (AFUNPTR)CheckArgumentTainted,
					IARG_THREAD_ID,
					IARG_RETURN_IP,
					IARG_CONTEXT,
					IARG_ADDRINT, "alloca",
					IARG_UINT32, 1,
					IARG_END);
				RTN_Close(mappingRtn);
			}
		}
	}
}

VOID ThreadStart(THREADID thread_id, CONTEXT *ctxt, INT32 flags, VOID *v)
{
	cerr << "[*] ThreadStart : thread_id - " << thread_id << ", esp - " << hex << PIN_GetContextReg(ctxt, REG_ESP) << endl;

	g_process_context.ThreadStart(thread_id, PIN_GetContextReg(ctxt, REG_ESP));
}

VOID ThreadFini(THREADID thread_id, const CONTEXT *ctxt, INT32 code, VOID* v)
{
	cerr << "[*] ThreadFini : thread_id - " << thread_id << endl;

	g_process_context.ThreadFini(thread_id);
}


void Fini(UINT32 code, void * v)
{
	g_process_context.Dump();
}


VOID Instruction(INS ins, VOID* v)
{
	// TODO : MOVSX, MOVDX ...
	// TODO : PUSH, POP
	// TODO : AND & 0
	// TODO : LEA
	// TODO : MULT, DIV

	BOOL result = FALSE;

	if (InstrumentMov(ins))
		return;
	
	if (InstrumentBinaryOperator(ins))
		return;

	if (InstrumentMovs(ins))
		return;

	if (InstrumentPush(ins))
		return;

	if (InstrumentPop(ins))
		return;

	if (InstrumentLea(ins))
		return;

	//if (InstrumentRet(ins))
	//	return;
}

VOID Trace(TRACE trace, VOID* v)
{
	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
	{
		INS ins = BBL_InsHead(bbl);
		g_signature_finder.Match(ins);
	}
}

BOOL InstrumentMov(INS ins)
{
	BOOL result = FALSE;
	OPCODE opcode = INS_Opcode(ins);

	if (INS_IsMov(ins)
			|| opcode == XED_ICLASS_MOVSX 
			|| opcode == XED_ICLASS_MOVZX)
	{
		result = TRUE;

		// MOV 처리
		if (INS_OperandIsReg(ins, 0) && INS_OperandIsReg(ins, 1))
		{
			// REG, REG
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovRegReg,
										IARG_THREAD_ID,
										IARG_INST_PTR,
										IARG_UINT32, REG(INS_OperandReg(ins, 0)),
										IARG_UINT32, REG(INS_OperandReg(ins, 1)),
										IARG_END);
		}

		else if (INS_OperandIsReg(ins, 0) && INS_OperandIsMemory(ins, 1))
		{
			// REG, MEM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovRegMem,
										IARG_THREAD_ID,
										IARG_INST_PTR,
										IARG_UINT32, REG(INS_OperandReg(ins, 0)),
										IARG_MEMORYREAD_EA,
										IARG_UINT32, INS_MemoryReadSize(ins),
										IARG_END);

		}

		else if (INS_OperandIsMemory(ins, 0) && INS_OperandIsReg(ins, 1))
		{
			// MEM, REG
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovMemReg,
										IARG_THREAD_ID,
										IARG_INST_PTR,
										IARG_MEMORYWRITE_EA,
										IARG_UINT32, INS_MemoryWriteSize(ins),
										IARG_UINT32, REG(INS_OperandReg(ins, 1)),
										IARG_END);
		}

		else if (INS_OperandIsMemory(ins, 0) && INS_OperandIsMemory(ins, 1))
		{
			// MEM MEM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovMemMem,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_MEMORYWRITE_EA,
				IARG_UINT32, INS_MemoryWriteSize(ins),
				IARG_MEMORYREAD_EA,
				IARG_UINT32, INS_MemoryReadSize(ins),
				IARG_END);
		}

		else if (INS_OperandIsReg(ins ,0) && INS_OperandIsImmediate(ins, 1))
		{
			// REG IMM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseFlushRegister,
										IARG_THREAD_ID,
										IARG_INST_PTR,
										IARG_UINT32, REG(INS_OperandReg(ins, 0)),
										IARG_END);
		}

		else if (INS_OperandIsMemory(ins, 0) && INS_OperandIsImmediate(ins, 1))
		{
			// MEM IMM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseFlushMemory,
										IARG_THREAD_ID,
										IARG_INST_PTR,
										IARG_MEMORYWRITE_EA,
										IARG_UINT32, INS_MemoryWriteSize(ins),
										IARG_END);
		}
		else
		{
			cerr << "[*] InstrumentMov" << endl;
			INS_Print(ins);
		}

	}
	return result;
}

BOOL InstrumentBinaryOperator(INS ins)
{
	BOOL			result = FALSE;
	char*			symbol = "";
	SYMBOLIC_TYPE	sym_type = SYMBOLIC_INFIX;

	switch(INS_Opcode(ins))
	{
	case XED_ICLASS_ADD:
	case XED_ICLASS_ADC:
		symbol = "+";
		break;

	case XED_ICLASS_SBB:
		symbol = "-";
		break;

	case XED_ICLASS_SUB:
		if (InstrumentXorFlush(ins))
			return TRUE;

		symbol = "-";
		break;

	case XED_ICLASS_AND:
		if (InstrumentAndFlush(ins))
			return TRUE;

		symbol = "&";
		break;

	case XED_ICLASS_OR:
		symbol = "|";
		break;

	case XED_ICLASS_XOR:
		if (InstrumentXorFlush(ins))
			return TRUE;
		
		symbol = "^";
		break;

	case XED_ICLASS_SHL:
		symbol = "<<";
		break;

	case XED_ICLASS_SHR:
	case XED_ICLASS_SAR:
		symbol = ">>";
		break;

	case XED_ICLASS_ROL:
		symbol = "<<<";
		break;

	case XED_ICLASS_ROR:
		symbol = ">>>";
		break;
	}

	if (strlen(symbol) > 0)
	{
		if (INS_OperandIsReg(ins, 0) && INS_OperandIsReg(ins, 1))
		{
			// REG, REG
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseBinaryOperatorRegReg,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_UINT32, sym_type,
				IARG_ADDRINT, symbol,
				IARG_UINT32, REG(INS_OperandReg(ins, 0)),
				IARG_REG_VALUE, REG(INS_OperandReg(ins, 0)),
				IARG_UINT32, REG(INS_OperandReg(ins, 1)),
				IARG_REG_VALUE, REG(INS_OperandReg(ins, 1)),
				IARG_END);
		}

		else if (INS_OperandIsReg(ins, 0) && INS_OperandIsMemory(ins, 1))
		{
			// REG, MEM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseBinaryOperatorRegMem,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_UINT32, sym_type,
				IARG_ADDRINT, symbol,
				IARG_UINT32, REG(INS_OperandReg(ins, 0)),
				IARG_REG_VALUE, REG(INS_OperandReg(ins, 0)),
				IARG_MEMORYREAD_EA,
				IARG_UINT32, INS_MemoryReadSize(ins),
				IARG_END);
		}

		else if (INS_OperandIsMemory(ins, 0) && INS_OperandIsReg(ins, 1))
		{
			// MEM, REG
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseBinaryOperatorMemReg,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_UINT32, sym_type,
				IARG_ADDRINT, symbol,
				IARG_MEMORYOP_EA, 0,
				IARG_UINT32, INS_MemoryWriteSize(ins),
				IARG_UINT32, REG(INS_OperandReg(ins, 1)),
				IARG_REG_VALUE, REG(INS_OperandReg(ins, 1)),
				IARG_END);
		}
		
		else if (INS_OperandIsReg(ins, 0) && INS_OperandIsImmediate(ins, 1))
		{
			// REG IMM
			ADDRINT imm = (ADDRINT)INS_OperandImmediate(ins, 1); // possible loss of data

			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseBinaryOperatorRegImm,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_UINT32, sym_type,
				IARG_ADDRINT, symbol,
				IARG_UINT32, REG(INS_OperandReg(ins, 0)),
				IARG_ADDRINT, imm,
				IARG_END);
		}

		else if (INS_OperandIsMemory(ins, 0) && INS_OperandIsImmediate(ins, 1))
		{
			// MEM IMM
			ADDRINT imm = (ADDRINT)INS_OperandImmediate(ins, 1);

			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseBinaryOperatorMemImm,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_UINT32, sym_type,
				IARG_ADDRINT, symbol,
				IARG_MEMORYWRITE_EA,
				IARG_UINT32, INS_MemoryWriteSize(ins),
				IARG_ADDRINT, imm,
				IARG_END);
		}
		
		else
		{
			cerr << "[*] InstrumentBinaryOperator" << endl;
			INS_Print(ins);
		}

		result = TRUE;
	}

	return result;
}

BOOL InstrumentAndFlush (INS ins)
{
	BOOL result = FALSE;

	// x and 0 = 0
	// 테인트 삭제 
	if (INS_OperandIsImmediate(ins, 1) && (INS_OperandImmediate(ins, 1) == 0x0))
	{
		if (INS_OperandIsReg(ins, 0))
		{
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseFlushRegister, 
										IARG_THREAD_ID, 
										IARG_INST_PTR, 
										IARG_UINT32, REG(INS_OperandReg(ins, 0)), 
										IARG_END);
			result = TRUE;
		}
		else if (INS_OperandIsMemory(ins, 0))
		{
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseFlushMemory,
										IARG_THREAD_ID,
										IARG_INST_PTR,
										IARG_MEMORYWRITE_EA,
										IARG_UINT32, INS_MemoryWriteSize(ins),
										IARG_END);
			result = TRUE;
		}
		else
		{
			cerr << "[*] InstrumentAndFlush" << endl;
			INS_Print(ins);
		}
	}
	return result;
}

BOOL InstrumentXorFlush(INS ins)
{
	
	if (INS_OperandIsReg(ins , 0) && INS_OperandIsReg(ins, 1) && (INS_OperandReg(ins, 0) == INS_OperandReg(ins, 1)))
	{
		INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseFlushRegister, 
										IARG_THREAD_ID, 
										IARG_INST_PTR, 
										IARG_UINT32, REG(INS_OperandReg(ins, 0)), 
										IARG_END);
		return TRUE;
	}
	else 
		return FALSE;
}

BOOL InstrumentMovs(INS ins)
{
	BOOL result = FALSE;
	switch(INS_Opcode(ins))
	{
		case XED_ICLASS_MOVSB:
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovs,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_MEMORYOP_EA, 0,
				IARG_MEMORYOP_EA, 1,
				IARG_UINT32, 1,
				IARG_END);
			result = TRUE;
			break;

		case XED_ICLASS_MOVSW:
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovs,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_MEMORYOP_EA, 0,
				IARG_MEMORYOP_EA, 1,
				IARG_UINT32, 2,
				IARG_END);
			result = TRUE;
			break;

		case XED_ICLASS_MOVSD:
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseMovs,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_MEMORYOP_EA, 0,
				IARG_MEMORYOP_EA, 1,
				IARG_UINT32, 4,
				IARG_END);
			result = TRUE;
			break;
	}

	return result;
}

BOOL InstrumentPush(INS ins)
{
	// TODO : PUSHA

	BOOL result = FALSE;

	switch(INS_Opcode(ins))
	{
	case XED_ICLASS_PUSH:
		if (INS_OperandIsReg(ins, 0))
		{
			// PUSH REG
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParsePushReg,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_CONTEXT,
				IARG_UINT32, REG(INS_OperandReg(ins, 0)),
				IARG_END);
		}
		else if (INS_OperandIsMemory(ins, 0))
		{
			// PUSH MEM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParsePushMem,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_CONTEXT,
				IARG_MEMORYREAD_EA,
				IARG_UINT32, INS_MemoryReadSize(ins),
				IARG_END);
		}
		else if (INS_OperandIsImmediate(ins, 0))
		{
			// PUSH IMM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParsePushImm,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_CONTEXT,
				IARG_END);
		}
		else
		{
			cerr << "[*] InstrumentPush" << endl;
			INS_Print(ins);
		}

		result = TRUE;
		break;
	}

	return result;

}

BOOL InstrumentPop(INS ins)
{
	// TODO : POPA

	BOOL result = FALSE;

	switch(INS_Opcode(ins))
	{
	case XED_ICLASS_POP:
		if (INS_OperandIsReg(ins, 0))
		{
			// POP REG
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParsePopReg,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_CONTEXT,
				IARG_UINT32, REG(INS_OperandReg(ins, 0)),
				IARG_END);

		}
		else if (INS_OperandIsMemory(ins, 0))
		{
			// POP MEM
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParsePopMem,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_CONTEXT,
				IARG_MEMORYWRITE_EA,
				IARG_UINT32, INS_MemoryWriteSize(ins),
				IARG_END);
		}
		else
		{
			cerr << "[*] InstrumentPop" << endl;
			INS_Print(ins);
		}

		result = TRUE;
		break;
	}

	return result;
}

BOOL InstrumentLea(INS ins)
{
	BOOL result = FALSE;

	switch(INS_Opcode(ins))
	{
	case XED_ICLASS_LEA:
		if (INS_OperandIsReg(ins, 0) && INS_OperandIsAddressGenerator(ins, 1))
		{
			INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseLeaReg,
				IARG_THREAD_ID,
				IARG_INST_PTR,
				IARG_CONTEXT,
				IARG_UINT32, REG(INS_OperandReg(ins, 0)),
				IARG_UINT32, REG(INS_OperandMemoryBaseReg(ins, 1)),
				IARG_UINT32, REG(INS_OperandMemoryIndexReg(ins, 1)),
				IARG_UINT32, INS_OperandMemoryDisplacement(ins, 1),
				IARG_UINT32, INS_OperandMemoryScale(ins, 1),
				IARG_END);
		}
		else
		{
			cerr << "[*] InstrumentLea" << endl;
			INS_Print(ins);
		}

		result = TRUE;
		break;
	}

	return result;
}

BOOL InstrumentRet(INS ins)
{
	BOOL result = FALSE;

	if (INS_IsRet(ins))
	{
		INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)ParseRet,
			IARG_THREAD_ID,
			IARG_INST_PTR,
			IARG_CONTEXT,
			IARG_END);

		result = TRUE;
	}

	return result;
}

VOID InstrumentFunction(INS ins)
{
	INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)CheckRegisterTainted,
					IARG_THREAD_ID,
					IARG_INST_PTR,
					IARG_ADDRINT, "alloca",
					IARG_UINT32, REG_EAX,
					IARG_END);
}