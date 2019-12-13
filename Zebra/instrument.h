#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_

#include <pin.H>
#include "defines.h"
#include "parse_instruction.h"

VOID Image(IMG img, VOID * v);

VOID ThreadStart(THREADID thread_id, CONTEXT *ctxt, INT32 flags, VOID *v);

VOID ThreadFini(THREADID thread_id, const CONTEXT *ctxt, INT32 code, VOID* v);

VOID Fini(UINT32 code, VOID * v);

// 인스트럭션 관련 함수
VOID Instruction(INS ins, VOID * v);

BOOL InstrumentBinaryOperator(INS ins);

BOOL InstrumentMov(INS ins);

BOOL InstrumentMovs(INS ins);

BOOL InstrumentXorFlush(INS ins);

BOOL InstrumentAndFlush (INS ins);

BOOL InstrumentPush(INS ins);

BOOL InstrumentPop(INS ins);

BOOL InstrumentLea(INS ins);

BOOL InstrumentRet(INS ins);


// 함수 찾기 관련
VOID Trace(TRACE trace, VOID* v);

VOID InstrumentFunction(INS ins);

#endif // _INSTRUMENT_H_