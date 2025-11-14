#pragma once

#include <stdint.h>

/// <summary>
/// Print an unsigned 16-bit integer with thousands groups.
/// </summary>
/// <param name="aNumber">Number to proint.</param>
/// <param name="separator">Thousands group separator character.</param>
/// <returns>Pointer to string of number.</returns>
char* FormatUint16WithSeparator(uint16_t const aNumber, char const separator);

/// <summary>
/// Print an unsigned 16-bit integer with thousands groups separated by the default separator character.
/// </summary>
/// <param name="aNumber">Number to proint.</param>
/// <returns>Pointer to string of number.</returns>
char* FormatUint16NumberWithDefaultSeparator(uint16_t const aNumber);

/// <summary>
/// Print an unsigned 16-bit integer.
/// </summary>
/// <param name="aNumber">Number to proint.</param>
/// <returns>Pointer to string of number.</returns>
char* FormatUint16Number(uint16_t const aNumber);
