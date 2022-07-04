#pragma once
#include <string>
#include <iostream>
#include "../LightweightTools/FunctionFactory.h"

static void TestFunctionFactory()
{
	using CapitalPrinter = CFunctionFactory<std::string, void()>;

	CapitalPrinter aCapitalPrinter;
	aCapitalPrinter.AddFunction("china", []() {std::cout << "Beijing" << std::endl; });
	aCapitalPrinter.AddFunction("usa", []() {std::cout << "Washington" << std::endl; });
	aCapitalPrinter.AddFunction("japan", []() {std::cout << "Tokyo" << std::endl; });
	aCapitalPrinter.AddFunction("france", []() {std::cout << "Paris" << std::endl; });

	aCapitalPrinter["china"]();
	aCapitalPrinter["france"]();
	aCapitalPrinter["japan"]();
}