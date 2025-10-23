#pragma once

#include <Windows.h>

/// <summary>
/// Print the version of the supplied module file.
/// </summary>
/// <param name="moduleName">Name of the module.</param>
/// <param name="fStdOut">Stdout file pointer.</param>
void PrintModuleVersion(const PWCHAR moduleName, FILE* fStdOut);