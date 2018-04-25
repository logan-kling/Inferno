#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Project2/Sim.h"
#include "../Project2/Sim.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			float test_the_test = 1000;
			float test_function_return = 0.0f;
			test_function_return = testMiles(50, 90);
			Assert::AreEqual(test_the_test, test_function_return);
		}

	};
}

namespace getDorSTest 
{
	TEST_CLASS(dTest) 
	{
	public:

		TEST_METHOD(DistanceTest1) {
			float distance_result = 0.0f;
			float test_equal = 0.0f;
			test_equal = ((1000.0f * 1.0f * log2((float)1.0f / 8.0f - (float)1.0f / 28.0f + 1.0f)) / (1.0f * (1.0f/50.0f + 1.0f) * log(1.0f + 1.0f) + 1.0f * 1.0f));
			distance_result = getDistance(1, 1, 1, 1, 1, 1);
			Assert::AreEqual(distance_result, test_equal);
		}
	};
}

namespace conversionTests
{
	TEST_CLASS(conversions)
	{
	public:

		TEST_METHOD(HpToWattTest) {
			double func_result = HpToWatt(1);
			double test_equal = (double)1 * (double)745.699872;
			Assert::AreEqual(func_result, test_equal);
		}

		TEST_METHOD(WattToHpTest) {
			double func_result = WattToHp(1);
			double test_equal = (double)1 * (double)0.00134102;
			Assert::AreEqual(func_result, test_equal);
		}
	};
}