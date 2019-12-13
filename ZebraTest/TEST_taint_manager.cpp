#include <gtest/gtest.h>
#include <Zebra/taint_manager.h>


class TaintManagerTest : public ::testing::Test
{
protected:
	virtual void SetUp() {
		input_taint.symbol = "test";
	}

protected:
	TaintManager taint_manager;
	Taint input_taint, output_taint;

	// TODO : pin.H 를 include하여 해당 라인 삭제
//	const int REG_EAX = 19;
};

TEST_F(TaintManagerTest, DoTaintRegister)
{
	taint_manager.DoTaintRegister(REG_EAX, input_taint);

	EXPECT_TRUE(taint_manager.IsTaintedRegister(REG_EAX, output_taint));
}

TEST_F(TaintManagerTest, UnTaintRegister)
{
	taint_manager.DoTaintRegister(REG_EAX, input_taint);
	taint_manager.UnTaintRegister(REG_EAX);

	EXPECT_FALSE(taint_manager.IsTaintedRegister(REG_EAX, output_taint));
}

TEST_F(TaintManagerTest, DoTaintMemory)
{
	taint_manager.DoTaintMemory(0, 4, input_taint);

	EXPECT_TRUE(taint_manager.IsTaintedMemory(0, 4, output_taint));
}

TEST_F(TaintManagerTest, UnTaintMemory1)
{
	taint_manager.DoTaintMemory(0, 4, input_taint);

	taint_manager.UnTaintMemory(0, 4);

	EXPECT_FALSE(taint_manager.IsTaintedMemory(0, 4, output_taint));
}

TEST_F(TaintManagerTest, UnTaintMemory2)
{
	taint_manager.DoTaintMemory(0, 4, input_taint);
	taint_manager.UnTaintMemory(2, 4);

	EXPECT_TRUE(taint_manager.IsTaintedMemory(0, 2, output_taint));
	EXPECT_FALSE(taint_manager.IsTaintedMemory(2, 2, output_taint));
}

TEST_F(TaintManagerTest, UnTaintMemory3)
{
	taint_manager.DoTaintMemory(1, 4, input_taint);
	taint_manager.UnTaintMemory(0, 3);
	EXPECT_TRUE(taint_manager.IsTaintedMemory(4, 2, output_taint));
	EXPECT_FALSE(taint_manager.IsTaintedMemory(0, 3, output_taint));
}

TEST_F(TaintManagerTest, UnTaintMemory4)
{
	taint_manager.DoTaintMemory(0, 4, input_taint);
	taint_manager.UnTaintMemory(1, 2);
	EXPECT_TRUE(taint_manager.IsTaintedMemory(0, 1, output_taint));
	EXPECT_TRUE(taint_manager.IsTaintedMemory(3, 1, output_taint));
	EXPECT_FALSE(taint_manager.IsTaintedMemory(1, 2, output_taint));
}