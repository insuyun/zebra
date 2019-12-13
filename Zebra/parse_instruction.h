#ifndef _PARSE_INSTRUCTION_H_
#define _PARSE_INSTRUCTION_H_

#include "process_context.h"
#include "taint.h"

typedef enum
{
	SYMBOLIC_PREFIX,
	SYMBOLIC_POSTFIX,
	SYMBOLIC_INFIX
} SYMBOLIC_TYPE;

Taint GenerateTaint(SYMBOLIC_TYPE sym_type, string symbol, 
						Taint& src_taint,
						ADDRINT src_val,
						Taint& dest_taint,
						ADDRINT dest_val);

// Mov
VOID ParseMovRegReg(THREADID thread_id,
					VOID* instruction_ptr, 
					REG dest,
					REG src);

VOID ParseMovRegMem(THREADID thread_id,
					VOID* instruction_ptr,
					REG dest_reg,
					ADDRINT src_addr,
					UINT32 src_size);

VOID ParseMovMemReg(THREADID thread_id,
					VOID* instruction_ptr,
					ADDRINT dest_addr,
					UINT32 dest_size,
					REG src_reg);

VOID ParseMovMemMem(THREADID thread_id,
					VOID* instruction_ptr,
					ADDRINT dest_addr,
					UINT32 dest_size,
					ADDRINT src_addr,
					UINT32 src_size);

// 이항 연산
VOID ParseBinaryOperatorRegReg(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol, 
								REG dest_reg,
								UINT32 dest_val,
								REG src_reg,
								UINT32 src_val);

VOID ParseBinaryOperatorRegMem(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol, 
								REG dest_reg,
								UINT32 dest_val,
								ADDRINT src_addr,
								UINT32 src_size);

VOID ParseBinaryOperatorMemReg(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol, 
								ADDRINT dest_addr,
								UINT32 dest_size,
								REG src_reg,
								UINT32 src_val);

VOID ParseBinaryOperatorRegImm(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol,
								REG dest_reg,
								ADDRINT imm);

VOID ParseBinaryOperatorMemImm(THREADID thread_id,
								VOID* instruction_ptr,
								SYMBOLIC_TYPE sym_type,
								char* symbol,
								ADDRINT dest_addr,
								UINT32 dest_size,
								ADDRINT imm);

// MOVS
VOID ParseMovs(THREADID thread_id,
				VOID* instruction_ptr,
				ADDRINT src_addr,
				ADDRINT dest_addr,
				UINT32 bytes);

// PUSH
VOID ParsePushReg(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt, 
					REG reg);

VOID ParsePushMem(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt, 
					ADDRINT addr,
					UINT32 size);

VOID ParsePushImm(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt);

// POP
VOID ParsePopReg(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt,
					REG reg);

VOID ParsePopMem(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt,
					ADDRINT addr,
					UINT32 size);

// LEA
VOID ParseLeaReg(THREADID thread_id,
					VOID* instruction_ptr,
					CONTEXT* ctxt,
					REG dest, 
					REG base, 
					REG index, 
					INT32 displacement,
					UINT32 scale);

// RET
VOID ParseRet(THREADID thread_id,
				VOID* instruction_ptr,
				CONTEXT* ctxt);


// 테인트 삭제
VOID ParseFlushRegister(THREADID thread_id, 
						VOID* instruction_ptr,
						REG reg);

VOID ParseFlushMemory(THREADID thread_id,
						VOID* instruction_ptr,
						ADDRINT addr,
						UINT32 size);

#endif // _PARSE_INSTRUCTION_H_