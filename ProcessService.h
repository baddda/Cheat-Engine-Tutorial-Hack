#ifndef PROCESS_SERVICE_H
#define PROCESS_SERVICE_H

#include <iostream>
#include <windows.h>
#include <vector>

class ProcessService
{
private:
	static ProcessService* instance;

	DWORD procId = 0;
	uintptr_t moduleBase = 0;
	HANDLE hProcess = 0;

	ProcessService() {};
	DWORD getProcId(const std::wstring& processName);
	uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName);
public:
	static ProcessService* getInstance();
	bool attach();
	bool solveStep2();
	bool solveStep3();
	bool solveStep4();
	bool solveStep5();
	bool solveStep6();
};

#endif