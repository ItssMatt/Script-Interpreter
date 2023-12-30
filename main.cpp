#include "script.h"

#include <windows.h>

int main(void) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hConsole, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConsole, dwMode);

	CScript sc = CScript("scripts/sub.script");
	if (sc.isValidScript()) {
		sc.Interpret();
		//sc.printStack();
		//sc.printVars();
		//sc.printInstructions();
	}

	CScript sum = CScript("scripts/sum.script");
	if (sum.isValidScript()) {
		sum.Interpret();
		//sum.printStack();
		//sum.printVars();
		//sum.printInstructions();
	}

	CScript mov = CScript("scripts/mov.script");
	if (mov.isValidScript()) {
		mov.Interpret();
		//mov.printStack();
		//mov.printVars();
		//mov.printInstructions();
	}

	CScript s_test = CScript("scripts/script_test.script");
	if (s_test.isValidScript()) {
		s_test.Interpret();
		//s_test.printStack();
		//s_test.printVars();
		//s_test.printInstructions();
	}

	std::cin.get();
}