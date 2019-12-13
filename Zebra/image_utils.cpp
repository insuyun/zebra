#include "image_utils.h"
#include "function_callback.h"

VOID IMG_PrintInstruction(IMG img, string image_name, ADDRINT offset)
{
	string name = IMG_Name(img);

	static int counter = 0;

	if (name.find(image_name) != string::npos)
	{

		for(SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
		{
			for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
			{
				RTN_Open(rtn);

				for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
				{
					if (IMG_StartAddress(img) + offset == INS_Address(ins))
					{
						cerr << "[*] IMG_PrintInstruction : " << hex << INS_Address(ins) << ", " << hex << IMG_StartAddress(img) << endl;
						INS_Print(ins);

						INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)CheckRegisterTainted, 
							IARG_THREAD_ID,
							IARG_INST_PTR,
							IARG_ADDRINT, "TAINT_CHECK",
							IARG_UINT32, REG_ESI,
							IARG_END);
					}
				}

				RTN_Close(rtn);

			}

		}
	}
}