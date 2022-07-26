#include "math/math.h"
#include "gtest/gtest.h"

TEST(TestGoogleTest,TestGoogleTest) {
	EXPECT_EQ(1, 1);
	EXPECT_EQ(5.f / 2.f, 1.f + 1.5f);
}


int main() {
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
}