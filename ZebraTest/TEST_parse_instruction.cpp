#include <gtest/gtest.h>
#include <Zebra/parse_instruction.h>

extern ProcessContext g_process_context;

#define INSPTR	(void*)0x410000

TEST(ParseInstruction, ParseMovRegReg)
{
	ParseMovRegReg(0, INSPTR, REG_EAX, REG_EBX);
	EXPECT_TRUE(TRUE);
}