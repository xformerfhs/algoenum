#include <Windows.h>
#include <stdio.h>
#include <VersionHelpers.h>

void PrintWindowsVersion() {
	OSVERSIONINFOEXW osvi;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);

#pragma warning( push )
#pragma warning( disable : 4996 )
	GetVersionExW((LPOSVERSIONINFOW)(&osvi));
#pragma warning( pop )

	wprintf(L"V%d.%d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.wServicePackMajor);
}