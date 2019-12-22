#include "ProcessService.h"

ProcessService* ProcessService::instance = 0;

ProcessService* ProcessService::getInstance()
{
	if (instance == 0)
	{
		instance = new ProcessService();
	}

	return instance;
}

void ProcessService::attach() {

	procId = getProcId(L"Tutorial-i386.exe");
	moduleBase = getModuleBaseAddress(procId, L"Tutorial-i386.exe");
	hProcess = 0;

	if (!OpenProcess(PROCESS_ALL_ACCESS, NULL, procId))
	{
		PrintLastErrorMessage();
	}
}

void ProcessService::solveStep2()
{
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x2015D0;
	std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	std::vector<unsigned int> ammoOffsets = { 0x480 };
	uintptr_t healthAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);
	std::cout << "healthAddr = " << "0x" << std::hex << healthAddr << std::endl;

	int healtValue = 0;
	ReadProcessMemory(hProcess, (LPVOID)healthAddr, &healtValue, sizeof(healtValue), nullptr);
	std::cout << "healtValue = " << std::dec << healtValue << std::endl;

	int newHealth = 1000;
	WriteProcessMemory(hProcess, (LPVOID*)healthAddr, &newHealth, sizeof(newHealth), nullptr);
	ReadProcessMemory(hProcess, (LPVOID*)healthAddr, &healtValue, sizeof(healtValue), nullptr);
	std::cout << "newHealth = " << std::dec << healtValue << std::endl;
}

DWORD ProcessService::getProcId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (Process32First(processesSnapshot, &processInfo)) {
		do
		{
			if (!processName.compare(processInfo.szExeFile))
			{
				CloseHandle(processesSnapshot);
				return processInfo.th32ProcessID;
			}
		} while (Process32Next(processesSnapshot, &processInfo));
	}

	CloseHandle(processesSnapshot);
	return 0;
}

uintptr_t ProcessService::getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				//OutputDebugString(L"Module Found");
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					//OutputDebugString(std::cout << "Found base address: " << "0x" << std::hex << modBaseAddr << std::endl);
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t ProcessService::findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (LPVOID*)(addr), &addr, sizeof(addr), 0);
		addr = addr + offsets[i];
	}
	return addr;
}