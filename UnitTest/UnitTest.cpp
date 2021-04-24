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
			Assert::IsTrue(str.Length() == strlen(str.c()), L"Best dragon Spyro dragon");
		}

		TEST_METHOD(IndexOf)
		{
			Assert::IsTrue(str.IndexOf("Best") == 0, L"Best");
			Assert::IsTrue(str.IndexOf("dragon") == 5, L"dragon");
			Assert::IsTrue(str.IndexOf("Spyro") == 12, L"Spyro");
			Assert::IsTrue(str.IndexOf("beast") == -1, L"beast");
			Assert::IsTrue(str.IndexOf("Best dragon Spyro dragon dragon") == -1, L"Best dragon Spyro dragon dragon");
		}

		TEST_METHOD(LastIndexOf)
		{
			Assert::IsTrue(str.LastIndexOf("dragon") == 18, L"dragon");
			Assert::IsTrue(str.LastIndexOf("beast") == -1, L"beast");
			Assert::IsTrue(str.LastIndexOf("Best dragon Spyro dragon dragon") == -1, L"Best dragon Spyro dragon dragon");
		}

		TEST_METHOD(Substring)
		{
			Assert::IsTrue(str.Substring(5, 6) == "dragon", L"dragon");
			Assert::IsTrue(str.Substring(5) == "dragon Spyro dragon", L"dragon Spyro dragon");
			Assert::IsTrue(str.Substring(5, 50) == "dragon Spyro dragon", L"50, dragon Spyro dragon");
		}

		TEST_METHOD(Replace)
		{
			Assert::IsTrue(str.Replace("Spyro", "Cynder") == "Best dragon Cynder dragon", L"Spyro -> Cynder");
			Assert::IsTrue(str.Replace("Spyro", "Cyn") == "Best dragon Cyn dragon", L"Spyro -> Cyn");
			Assert::IsTrue(str.Replace("dragon", "drag") == "Best drag Spyro drag", L"dragon -> drag");
		}

		TEST_METHOD(Remove)
		{
			Assert::IsTrue(str.Remove("Spyro") == "Best dragon  dragon", L"Spyro");
			Assert::IsTrue(str.Remove("dragon") == "Best  Spyro ", L"dragon");
		}

		TEST_METHOD(Fit)
		{
			String s = "dragon";

			Assert::IsTrue(s.Fit(10, '-') == "dragon----", L"dragon----");
			Assert::IsTrue(s.Fit(4, '-') == "drag", L"drag");
			Assert::IsTrue(s.Fit(-10, '-') == "----dragon", L"----dragon");
			Assert::IsTrue(s.Fit(-4, '-') == "agon", L"agon");
		}

		TEST_METHOD(LowerUpper)
		{
			String s = "Dragon";

			Assert::IsTrue(s.ToLower() == "dragon", L"dragon");
			Assert::IsTrue(s.ToUpper() == "DRAGON", L"DRAGON");
		}

		TEST_METHOD(Trim)
		{
			String s = "   dragon   ";

			Assert::IsTrue(s.TrimStart() == "dragon   ", L"dragon   ");
			Assert::IsTrue(s.TrimEnd() == "   dragon", L"   dragon");
			Assert::IsTrue(s.Trim() == "dragon", L"dragon");
		}

		TEST_METHOD(Contains)
		{
			Assert::IsTrue(str.StartsWith("Best"), L"Starts with Best");
			Assert::IsTrue(str.EndsWith("dragon"), L"Ends with dragon");
			Assert::IsTrue(str.Contains("Spyro"), L"Contains Spyro");
		}

		TEST_METHOD(Split)
		{
			String s = "my test  dragon";

			Assert::IsTrue(s.Split(" ") == List<String>{"my", "test", "", "dragon"}, L"keep empty");
			Assert::IsTrue(s.Split(" ", true) == List<String>{"my", "test", "dragon"}, L"drop empty");
		}
	};
}
