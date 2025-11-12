#pragma once

/// <summary>
/// Convert a wide character string into a string for the console code page.
/// </summary>
/// <param name="wideMessage">UTF-16 message.</param>
/// <returns>Pointer to message converted to the console code page.</returns>
char* AsConsoleCodePageString(const wchar_t* const wideMessage);

/// <summary>
/// Prints a wide character formatted string to the console.
/// </summary>
/// <param name="stream">Output stream.</param>
/// <param name="format">Format.</param>
/// <param name="">Parameters for format.</param>
void PrintWideFormatToConsole(FILE* const stream, const wchar_t* const format, ...);