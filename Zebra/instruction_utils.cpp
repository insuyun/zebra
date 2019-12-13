#include "instruction_utils.h"

VOID INS_Print(INS ins)
{
	UINT32 count = INS_OperandCount(ins), i = 0;
	
	cerr <<"[*] INS_Print : " << INS_Disassemble(ins) << endl;

	for (i = 0; i < count; i++)
	{
		cerr << "[**] Operand : " << i << endl;
		if (INS_OperandIsAddressGenerator(ins, i))
		{
			cerr << "[***] AddressGenerator" << endl;
			if (REG_valid(INS_OperandMemoryBaseReg(ins, i)))
				cerr << "[***] BaseReg : " << REG_StringShort(INS_OperandMemoryBaseReg(ins, i)) << endl;

			if (REG_valid(INS_OperandMemoryIndexReg(ins, i)))
				cerr << "[***] IndexReg : " << REG_StringShort(INS_OperandMemoryIndexReg(ins, i)) << endl;

			cerr << "[***] Displacement : " << INS_OperandMemoryDisplacement(ins, i) << endl;
			cerr << "[***] Scale : " << INS_OperandMemoryScale(ins, i) << endl;
		}

		if (INS_OperandIsBranchDisplacement(ins, i))
		{
			cerr << "[**] BranchDisplacement" << endl;
		}

		if (INS_OperandIsFixedMemop(ins, i))
		{
			cerr << "[**] FixedMemop" << endl;
		}

		if (INS_OperandIsImmediate(ins, i))
		{
			cerr << "[**] Immediate : "<< hex << INS_OperandImmediate(ins, i) << endl;
		}

		if (INS_OperandIsImplicit(ins, i))
		{
			cerr << "[**] Implicit " << endl;
		}

		if (INS_OperandIsMemory(ins, i))
		{
			cerr << "[**] Memory" << endl;
		}

		if (INS_OperandIsReg(ins, i))
		{
			cerr << "[**] Reg : " << REG_StringShort(INS_OperandReg(ins, i)) << endl;
		}
		cerr << endl;
	}
}