#include "pch.h"
#include "CppUnitTest.h"
#include "../Engine/String.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HexaTest
{
	TEST_CLASS(EngineStringTest)
	{
		const String str = "Best dragon Spyro dragon";

	public:
		TEST_METHOD(WString)
		{
			Assert::IsTrue(false, str.wc());
		}

		TEST_METHOD(Length)
		{
			Assert::IsTrue(str.length_() == strlen(str.c()), L"Best dragon Spyro dragon");
		}

		TEST_METHOD(IndexOf)
		{
			Assert::IsTrue(str.index_of("Best") == 0, L"Best");
			Assert::IsTrue(str.index_of("dragon") == 5, L"dragon");
			Assert::IsTrue(str.index_of("Spyro") == 12, L"Spyro");
			Assert::IsTrue(str.index_of("beast") == -1, L"beast");
			Assert::IsTrue(str.index_of("Best dragon Spyro dragon dragon") == -1, L"Best dragon Spyro dragon dragon");
		}

		TEST_METHOD(LastIndexOf)
		{
			Assert::IsTrue(str.last_index_of("dragon") == 18, L"dragon");
			Assert::IsTrue(str.last_index_of("beast") == -1, L"beast");
			Assert::IsTrue(str.last_index_of("Best dragon Spyro dragon dragon") == -1, L"Best dragon Spyro dragon dragon");
		}

		TEST_METHOD(Substring)
		{
			Assert::IsTrue(str.substring(5, 6) == "dragon", L"dragon");
			Assert::IsTrue(str.substring(5) == "dragon Spyro dragon", L"dragon Spyro dragon");
			Assert::IsTrue(str.substring(5, 50) == "dragon Spyro dragon", L"50, dragon Spyro dragon");
		}

		TEST_METHOD(Replace)
		{
			Assert::IsTrue(str.replace("Spyro", "Cynder") == "Best dragon Cynder dragon", L"Spyro -> Cynder");
			Assert::IsTrue(str.replace("Spyro", "Cyn") == "Best dragon Cyn dragon", L"Spyro -> Cyn");
			Assert::IsTrue(str.replace("dragon", "drag") == "Best drag Spyro drag", L"dragon -> drag");
		}

		TEST_METHOD(Remove)
		{
			Assert::IsTrue(str.remove("Spyro") == "Best dragon  dragon", L"Spyro");
			Assert::IsTrue(str.remove("dragon") == "Best  Spyro ", L"dragon");
		}

		TEST_METHOD(Fit)
		{
			String s = "dragon";

			Assert::IsTrue(s.fit(10, '-') == "dragon----", L"dragon----");
			Assert::IsTrue(s.fit(4, '-') == "drag", L"drag");
			Assert::IsTrue(s.fit(-10, '-') == "----dragon", L"----dragon");
			Assert::IsTrue(s.fit(-4, '-') == "agon", L"agon");
		}

		TEST_METHOD(LowerUpper)
		{
			String s = "Dragon";

			Assert::IsTrue(s.to_lower() == "dragon", L"dragon");
			Assert::IsTrue(s.to_upper() == "DRAGON", L"DRAGON");
		}

		TEST_METHOD(Trim)
		{
			String s = "   dragon   ";

			Assert::IsTrue(s.trim_start() == "dragon   ", L"dragon   ");
			Assert::IsTrue(s.trim_end() == "   dragon", L"   dragon");
			Assert::IsTrue(s.trim() == "dragon", L"dragon");
		}

		TEST_METHOD(Contains)
		{
			Assert::IsTrue(str.starts_with("Best"), L"Starts with Best");
			Assert::IsTrue(str.ends_with("dragon"), L"Ends with dragon");
			Assert::IsTrue(str.contains("Spyro"), L"Contains Spyro");
		}

		TEST_METHOD(Split)
		{
			String s = "my test  dragon";

			Assert::IsTrue(s.split(" ") == List<String>{"my", "test", "", "dragon"}, L"keep empty");
			Assert::IsTrue(s.split(" ", true) == List<String>{"my", "test", "dragon"}, L"drop empty");
		}
	};
}
