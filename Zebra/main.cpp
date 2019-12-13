#include <pin.H>

#include <iostream>
#include <fstream>

#include "process_context.h"
#include "instrument.h"

// 전역 변수
ProcessContext g_process_context;

int main(int argc, char * argv[])
{
    PIN_InitSymbols();

    if (PIN_Init(argc, argv)) 
		return 0;

	setlocale(LC_ALL, "");

	PIN_SetSyntaxIntel();

	// 실제 Taint analysis가 이루어지는 instrumentation function
	IMG_AddInstrumentFunction(Image, 0);
	INS_AddInstrumentFunction(Instruction, 0);
	TRACE_AddInstrumentFunction(Trace, 0);

	PIN_AddThreadStartFunction(ThreadStart, 0);
	PIN_AddThreadFiniFunction(ThreadFini, 0);

	PIN_AddFiniFunction((FINI_CALLBACK)Fini, 0);

    PIN_StartProgram();
    
    return 0;

}