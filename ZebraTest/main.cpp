#include <gtest/gtest.h>
#include <Zebra/process_context.h>

ProcessContext g_process_context;

#define STACK_BASE	0x120000

void InitProcessContext()
{
	g_process_context.ThreadStart(0, STACK_BASE);
	g_process_context.ThreadStart(1, STACK_BASE);
	g_process_context.ThreadStart(2, STACK_BASE);
}

void main(int argc, char** argv)
{
	InitProcessContext();


	::testing::InitGoogleTest(&argc, argv);	
	RUN_ALL_TESTS();
}