//
// SPDX-FileCopyrightText: Copyright 2024 Frank Schwab
//
// SPDX-License-Identifier: Apache-2.0
//
// SPDX-FileType: SOURCE
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
//
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Purpose: Printing of texts for error codes.
//
// Author: Frank Schwab
//
// Version: 1.0.1
//
// Change history:
//    2024-11-12: V1.0.0: Created.
//    2024-11-13: V1.0.1: Created.
//

#include <Windows.h>
#include <stdio.h>

#include "ApiErrorHandler.h"

// ******** Private data ********

// Hopefully the file path will be less than MAX_PATH - 1 characters...
WCHAR fileName[MAX_PATH];

// ******** Public methods ********

/// <summary>
/// Print the version of the supplied module file.
/// </summary>
/// <param name="moduleName">Name of the module.</param>
/// <param name="fStdOut">Stdout file pointer.</param>
void PrintModuleVersion(const PWCHAR moduleName, FILE* fStdOut) {
   // Name of this function for error messages.
   const PWCHAR functionName = L"PrintModuleVersion";

   // Getting the version of a loaded module is ridiciously complicated.
   // And each step can fail...
   
   // 1. Get the module handle from the module name.
   HMODULE hModule = GetModuleHandleW(moduleName);
   if (hModule == INVALID_HANDLE_VALUE) {
      PrintLastError(functionName, L"GetModuleHandle");
      return;
   }

   // 2. Get the file name from the module handle.
   
   GetModuleFileNameW(hModule, fileName, sizeof(fileName)/sizeof(WCHAR));
   if (GetLastError() != 0) {
      PrintLastError(functionName, L"GetModuleFileName");
      return;
   }

   // 3. Get the size of the version information for the file.

   // The size of the file version information.
   DWORD fileVersionInfoSize = GetFileVersionInfoSizeW(fileName, NULL);
   if (fileVersionInfoSize == 0) {
      PrintLastError(functionName, L"GetFileVersionInfoSize");
      return;
   }

   // 3.1 Allocate memory to store the opaque version information blob.

   // The handle of the process heap.
   HANDLE hHeap = GetProcessHeap();
   if (hHeap == NULL) {
      PrintLastError(functionName, L"GetProcessHeap");
      return;
   }

   // The pointer to the memory that will hold the opaque file version information blob.
   LPVOID pFileVersionInfo = HeapAlloc(hHeap, 0, fileVersionInfoSize);
   if (pFileVersionInfo == NULL) {
      PrintLastError(functionName, L"HeapAlloc");
      return;
   }

   // 3.2 Get the opaque version information blob.
   //     This is bizarre: The version information has no known structure.
   if (GetFileVersionInfoW(fileName, 0, fileVersionInfoSize, pFileVersionInfo) == FALSE) {
      PrintLastError(functionName, L"GetFileVersionInfo");
      HeapFree(hHeap, 0, pFileVersionInfo);
      return;
   }

   // 4. Copy the version numbers from the opaque version information blob.
   //    The call has a strange and bizarre interface. What is this "\" to query?
   //    Why is this not just some structure?

   // This is where the file version numbers will be copied to.
   VS_FIXEDFILEINFO* pfi;
   UINT fiLength;
   if (VerQueryValueW(pFileVersionInfo, L"\\", &pfi, &fiLength) == FALSE) {
      PrintLastError(functionName, L"VerQueryValue");
      HeapFree(hHeap, 0, pFileVersionInfo);
      return;
   }

   // 5. Release the memory of the version information blob.
   HeapFree(hHeap, 0, pFileVersionInfo);

   // Hooray! Done! We can print the version information.
   // But wait... The version is hidden in DWORDs which we have to untangle ourselves.
   // This is so bizarre...
   fwprintf(fStdOut,
            L"V%d.%d.%d.%d",
            (pfi->dwProductVersionMS >> 16) & 0xffff,
            pfi->dwProductVersionMS & 0xffff,
            (pfi->dwProductVersionLS >> 16) & 0xffff,
            pfi->dwProductVersionLS & 0xffff);
}
