#include "ProcessService.h"
#include "Util.h"
#include <tlhelp32.h>

ProcessService* ProcessService::instance = 0;

ProcessService* ProcessService::getInstance()
{
	if (instance == 0)
	{
		instance = new ProcessService();
	}

	return instance;
}

bool ProcessService::attach() {

	procId = getProcId(L"Tutorial-i386.exe");
	moduleBase = getModuleBaseAddress(procId, L"Tutorial-i386.exe");

	if (!procId)
	{
		std::clog << "Process not found" << std::endl;
		return false;
	}

	if (!moduleBase)
	{
		std::clog << "Base address not found" << std::endl;
		return false;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	if (!hProcess)
	{
		std::clog << "Could not open process" << std::endl;
		PrintLastErrorMessage();
		return false;
	}

	return true;
}

bool ProcessService::solveStep2()
{
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x2015D0;

	std::vector<unsigned int> healthOffsets = { 0x480 };
	uintptr_t healthAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffsets);
	std::clog << "Health address: " << "0x" << std::hex << healthAddr << std::endl;

	int healtValue = 0;
	ReadProcessMemory(hProcess, (LPVOID)healthAddr, &healtValue, sizeof(healtValue), nullptr);

	int newHealth = 1000;
	WriteProcessMemory(hProcess, (LPVOID*)healthAddr, &newHealth, sizeof(newHealth), nullptr);

	ReadProcessMemory(hProcess, (LPVOID*)healthAddr, &healtValue, sizeof(healtValue), nullptr);
	std::clog << "New health value: " << healtValue << std::endl;

	return true;
}

bool ProcessService::solveStep3()
{
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x2015E0;

	std::vector<unsigned int> healthOffsets = { 0x484 };
	uintptr_t healthAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffsets);
	std::clog << "Health address: " << "0x" << std::hex << healthAddr << std::endl;

	int healtValue = 0;
	ReadProcessMemory(hProcess, (LPVOID)healthAddr, &healtValue, sizeof(healtValue), nullptr);

	int newHealth = 5000;
	WriteProcessMemory(hProcess, (LPVOID*)healthAddr, &newHealth, sizeof(newHealth), nullptr);

	ReadProcessMemory(hProcess, (LPVOID*)healthAddr, &healtValue, sizeof(healtValue), nullptr);
	std::clog << "New health value: " << healtValue << std::endl;

	return true;
}

bool ProcessService::solveStep4()
{
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x201600;

	std::vector<unsigned int> healthOffsets = { 0x494 };
	uintptr_t healthAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, healthOffsets);
	std::clog << "Health address: " << "0x" << std::hex << healthAddr << std::endl;

	float healtValue = 0;
	ReadProcessMemory(hProcess, (LPVOID)healthAddr, &healtValue, sizeof(healtValue), nullptr);

	float newHealth = 5000;
	WriteProcessMemory(hProcess, (LPVOID*)healthAddr, &newHealth, sizeof(newHealth), nullptr);

	ReadProcessMemory(hProcess, (LPVOID*)healthAddr, &healtValue, sizeof(healtValue), nullptr);
	std::clog << "New Health Value: " << healtValue << std::endl;

	std::vector<unsigned int> ammoOffsets = { 0x498 };
	uintptr_t ammoAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);
	std::clog << "Ammo address: " << "0x" << std::hex << ammoAddr << std::endl;

	double ammoValue = 0;
	ReadProcessMemory(hProcess, (LPVOID)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);

	double newAmmo = 5000;
	WriteProcessMemory(hProcess, (LPVOID*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);

	ReadProcessMemory(hProcess, (LPVOID*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	std::clog << "New vmmo value: " << ammoValue << std::endl;

	return true;
}

bool ProcessService::solveStep5()
{
	uintptr_t codeAddress = moduleBase + 0x24E98;
	BOOL success = 0;
	byte newOpcode[2] = { 0x90, 0x90 };
	DWORD oldprotect;

	std::clog << "Code address: " << "0x" << std::hex << codeAddress << std::endl;

	success = VirtualProtectEx(hProcess, (LPVOID)codeAddress, std::size(newOpcode), PAGE_EXECUTE_READWRITE, &oldprotect);
	if (!success)
	{
		std::clog << "VirtualProtectEx failed" << std::endl;
		PrintLastErrorMessage();
		return false;
	}

	byte opcode = 0;
	success = ReadProcessMemory(hProcess, (LPVOID)codeAddress, &opcode, sizeof(opcode), nullptr);
	if (!success)
	{
		std::clog << "ReadProcessMemory failed" << std::endl;
		PrintLastErrorMessage();
		return false;
	}
	std::clog << "Read opcode: " << "0x" << unsigned(opcode) << std::endl;

	success = WriteProcessMemory(hProcess, (LPVOID*)codeAddress, newOpcode, std::size(newOpcode), nullptr);
	if (!success)
	{
		std::clog << "WriteProcessMemory failed" << std::endl;
		PrintLastErrorMessage();
		return false;
	}

	success = VirtualProtectEx(hProcess, (LPVOID)codeAddress, std::size(newOpcode), oldprotect, &oldprotect);
	if (!success)
	{
		std::clog << "VirtualProtectEx failed" << std::endl;
		PrintLastErrorMessage();
		return false;
	}

	return true;
}

bool ProcessService::solveStep6()
{
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x201630;

	std::vector<unsigned int> valueOffsets = { 0x0 };
	uintptr_t valueAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, valueOffsets);
	std::clog << "Health address: " << "0x" << std::hex << valueAddr << std::endl;

	int value = 0;
	ReadProcessMemory(hProcess, (LPVOID)valueAddr, &value, sizeof(value), nullptr);

	int newValue = 5000;
	WriteProcessMemory(hProcess, (LPVOID*)valueAddr, &newValue, sizeof(newValue), nullptr);

	ReadProcessMemory(hProcess, (LPVOID*)valueAddr, &value, sizeof(value), nullptr);
	std::clog << "New Health Value: " << value << std::endl;

	return true;
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
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					std::clog << "Found base address: " << "0x" << std::hex << modBaseAddr << std::endl;
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