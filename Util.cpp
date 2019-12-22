#include "Util.h"
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

void PrintLastErrorMessage()
{
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return;

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	LocalFree(messageBuffer);

	OutputDebugStringA(message.c_str());
}