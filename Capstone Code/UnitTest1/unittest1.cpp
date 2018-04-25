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