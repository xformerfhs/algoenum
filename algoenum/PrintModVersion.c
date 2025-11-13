//
// SPDX-FileCopyrightText: Copyright 2024-2025 Frank Schwab
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
// Version: 2.0.0
//
// Change history:
//    2024-11-12: V1.0.0: Created.
//    2024-11-13: V1.0.1: Small change.
//    2025-11-12: V2.0.0: Print to console in console code page.
//

#define _CRT_DISABLE_PERFCRIT_LOCKS 1

#include <Windows.h>
#include <stdio.h>

#include "ApiErrorHandler.h"
#include "NumberFormatter.h"

// ******** Private data ********

// Hopefully the file path will be less than MAX_PATH - 1 characters...
CHAR fileName[MAX_PATH];

// ******** Public methods ********

/// <summary>
/// Print the version of the supplied module file.
/// </summary>
/// <param name="moduleName">Name of the module.</param>
/// <param name="fStdOut">Stdout file pointer.</param>
void PrintModuleVersion(const PCHAR moduleName, FILE* fStdOut) {
   // Name of this function for error messages.
   const PCHAR functionName = "PrintModuleVersion";

   // Getting the version of a loaded module is ridiciously complicated.
   // And each step can fail...

   // 1. Get the module handle from the module name.
   HMODULE hModule = GetModuleHandleA(moduleName);
   if (hModule == INVALID_HANDLE_VALUE) {
      PrintLastError(functionName, "GetModuleHandle");
      return;
   }

   // 2. Get the file name from the module handle.   
   GetModuleFileNameA(hModule, fileName, sizeof(fileName));
   if (GetLastError() != 0) {
      PrintLastError(functionName, "GetModuleFileName");
      return;
   }

   // 3. Get the size of the version information for the file.
   DWORD fileVersionInfoSize = GetFileVersionInfoSizeA(fileName, NULL);
   if (fileVersionInfoSize == 0) {
      PrintLastError(functionName, "GetFileVersionInfoSize");
      return;
   }

   // 4. Allocate memory to store the opaque version information blob.
   HANDLE hHeap = GetProcessHeap();
   if (hHeap == NULL) {
      PrintLastError(functionName, "GetProcessHeap");
      return;
   }

   // The pointer to the memory that will hold the opaque file version information blob.
   LPVOID pFileVersionInfo = HeapAlloc(hHeap, 0, fileVersionInfoSize);
   if (pFileVersionInfo == NULL) {
      PrintLastError(functionName, "HeapAlloc");
      return;
   }

   // 5. Get the opaque version information blob.
   //    This is bizarre: The version information has no known structure.
   if (GetFileVersionInfoA(fileName, 0, fileVersionInfoSize, pFileVersionInfo) == FALSE) {
      PrintLastError(functionName, "GetFileVersionInfo");
      HeapFree(hHeap, 0, pFileVersionInfo);
      return;
   }

   // 6. Copy the version numbers from the opaque version information blob.
   //    The call has a strange and bizarre interface.
   //    "\" means to query the root block of the version information.
   //    Why is this not just some structure?

   // This is where the file version numbers will be copied to.
   VS_FIXEDFILEINFO* pfi;
   UINT fiLength;
   if (VerQueryValueA(pFileVersionInfo, "\\", &pfi, &fiLength) == FALSE) {
      PrintLastError(functionName, "VerQueryValue");
      HeapFree(hHeap, 0, pFileVersionInfo);
      return;
   }

   // 7. Release the memory of the version information blob.
   HeapFree(hHeap, 0, pFileVersionInfo);


   // Hooray! Done! We can print the version information.
   // But wait... The version is hidden in DWORDs which we have to untangle ourselves.
   // This is so bizarre...
   _fputc_nolock('V', fStdOut);
   fputs(FormatUint16Number((pfi->dwProductVersionMS >> 16) & 0xffff), fStdOut);
   _fputc_nolock('.', fStdOut);
   fputs(FormatUint16Number(pfi->dwProductVersionMS & 0xffff), fStdOut);
   _fputc_nolock('.', fStdOut);
   fputs(FormatUint16Number((pfi->dwProductVersionLS >> 16) & 0xffff), fStdOut);
   _fputc_nolock('.', fStdOut);
   fputs(FormatUint16Number(pfi->dwProductVersionLS & 0xffff), fStdOut);
}
