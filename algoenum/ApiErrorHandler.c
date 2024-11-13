//
// SPDX-FileCopyrightText: Copyright 2023-2024 Frank Schwab
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
// Version: 1.0.0
//
// Change history:
//    2023-11-18: V1.0.0: Created.
//

#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

// ======== Private constants ========

#define MESSAGE_BUFFER_LENGTH 256

// ======== Private variables ========

WCHAR messageBuffer[MESSAGE_BUFFER_LENGTH];

// ======== Private methods ========

/// <summary>
/// Get the text for an NTSTATUS.
/// </summary>
/// <param name="errorNumber">NTSTATUS to get the text for.</param>
/// <returns>Length of message text. A value of 0 indicates that no message could be found.</returns>
DWORD getNtStatusErrorMessage(const DWORD errorNumber) {
	// For *all* NTSTATUS codes it is necessary to look them up in "ntdll.dll"!
	HMODULE ntdllModule = GetModuleHandleW(L"ntdll.dll");
	DWORD msgLen = FormatMessageW(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
											ntdllModule,
											errorNumber,
											MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
											messageBuffer,
											MESSAGE_BUFFER_LENGTH,
											NULL);
	return msgLen;
}

/// <summary>
/// Get the text for a Windows error code (GetLastError).
/// </summary>
/// <param name="errorNumber">Error code.</param>
/// <returns>Length of message text. A value of 0 indicates that no message could be found.</returns>
DWORD getSystemErrorMessage(const DWORD errorNumber) {
	DWORD msgLen = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
											NULL,
											errorNumber,
											MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
											messageBuffer,
											MESSAGE_BUFFER_LENGTH,
											NULL);
	return msgLen;
}

/// <summary>
/// Print an error message for a specified function and error number.
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
/// <param name="errorNumber">Error number.</param>
/// <param name="isNtStatus">Is the error number an NTSTATUS.</param>
void printError(WCHAR const* functionName, WCHAR const* apiName, DWORD const errorNumber, BOOL const isNtStatus) {
	DWORD msgLen;
	if (isNtStatus == FALSE) {
		msgLen = getSystemErrorMessage(errorNumber);
	} else {
		msgLen = getNtStatusErrorMessage(errorNumber);
	}

	DWORD le = 0;
	if (msgLen == 0)
		le = GetLastError();

	fwprintf(stderr, L"Function \"%s\", API function \"%s\" failed with error %d (0x%08x): ", 
				functionName, 
				apiName,
				errorNumber,
				errorNumber);

	if (msgLen > 0) {
		fputws(messageBuffer, stderr);
	} else {
		fwprintf(stderr, L"Could not get error message (FormatMessage error code = %d)\n", le);
	}
}

// ======== Public methods ========

/// <summary>
/// Print the error message for a Windows error code.
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
/// <param name="errorNumber">Error number.</param>
void PrintWinError(WCHAR const* functionName, WCHAR const* apiName, DWORD const errorNumber) {
	printError(functionName, apiName, errorNumber, FALSE);
}

/// <summary>
/// Print the error message for last faining Windows API function (GetLastError).
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
void PrintLastError(WCHAR const* functionName, WCHAR const* apiName) {
	PrintWinError(functionName, apiName, GetLastError());
}

/// <summary>
/// Print the error message for an NTSTATUS.
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
/// <param name="errorStatus">NTSTATUS of failing function.</param>
void PrintNtStatus(WCHAR const* functionName, WCHAR const* apiName, NTSTATUS const errorStatus) {
	printError(functionName, apiName, (DWORD)errorStatus, TRUE);
}
