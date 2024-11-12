#include <Windows.h>
#include <stdio.h>

#include "ApiErrorHandler.h"

void PrintModuleVersion(WCHAR const * moduleName) {
   WCHAR const * functionName = L"ListCurrentProcessModules";

   HMODULE hBCrypt = GetModuleHandleW(moduleName);
   if (hBCrypt == INVALID_HANDLE_VALUE) {
      PrintLastError(functionName, L"GetModuleHandle");;
      return;
   }

   WCHAR fileName[1024];

   DWORD fileNameLength = GetModuleFileNameW(hBCrypt, fileName, sizeof(fileName)/sizeof(WCHAR));
   if (fileNameLength == 0) {
      PrintLastError(functionName, L"GetModuleFileName");;
      return;
   }

   DWORD fileVersionInfoSize = GetFileVersionInfoSizeW(fileName, NULL);
   if (fileVersionInfoSize == 0) {
      PrintLastError(functionName, L"GetFileVersionInfoSize");;
      return;
   }

   HANDLE hHeap = GetProcessHeap();
   if (hHeap == NULL) {
      PrintLastError(functionName, L"GetProcessHeap");;
      return;
   }

   LPVOID pFileVersionInfo = HeapAlloc(hHeap, 0, fileVersionInfoSize);
   if (pFileVersionInfo == NULL) {
      PrintLastError(functionName, L"HeapAlloc");;
      return;
   }

   if (GetFileVersionInfoW(fileName, 0, fileVersionInfoSize, pFileVersionInfo) == FALSE) {
      PrintLastError(functionName, L"GetFileVersionInfo");
      HeapFree(hHeap, 0, pFileVersionInfo);
      return;
   }

   VS_FIXEDFILEINFO* pfi;
   UINT fiLength;
   if (VerQueryValueW(pFileVersionInfo, L"\\", &pfi, &fiLength) == FALSE) {
      PrintLastError(functionName, L"VerQueryValue");
      HeapFree(hHeap, 0, pFileVersionInfo);
      return;
   }

   HeapFree(hHeap, 0, pFileVersionInfo);

   wprintf(L"V%d.%d.%d.%d",
           (pfi->dwProductVersionMS >> 16) & 0xffff,
           pfi->dwProductVersionMS & 0xffff,
           (pfi->dwProductVersionLS >> 16) & 0xffff,
           pfi->dwProductVersionLS & 0xffff);
}
