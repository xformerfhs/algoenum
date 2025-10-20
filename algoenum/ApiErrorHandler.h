#pragma once

#include <Windows.h>

/// <summary>
/// Print the error message for a Windows error code.
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
/// <param name="errorNumber">Error number.</param>
void PrintWinError(const PWCHAR functionName, const PWCHAR apiName, const DWORD errorNumber);

/// <summary>
/// Print the error message for last faining Windows API function (GetLastError).
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
void PrintLastError(const PWCHAR functionName, const PWCHAR apiName);

/// <summary>
/// Print the error message for an NTSTATUS.
/// </summary>
/// <param name="functionName">Name of the function calling the failing Windows API function.</param>
/// <param name="apiName">Name of the failing Windows API function.</param>
/// <param name="errorStatus">NTSTATUS of failing function.</param>
void PrintNtStatus(const PWCHAR functionName, const PWCHAR apiName, const NTSTATUS errorStatus);
