//
// SPDX-FileCopyrightText: Copyright 2025 Frank Schwab
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
// Author: Frank Schwab
//
// Version: 1.0.0
//
// Change history:
//    2025-11-13: V1.0.0: Created.
//

#include <stdint.h>

// ******** Private constants ********

/// Default thousands group separator character.
#define DEFAULT_SEPARATOR ','

/// Size of the buffer for formatting numbers with thousands grouping separators.
#define BUFFER_SIZE 20

// ******** Private variables ********

/// Buffer for formatting numbers with thousands grouping separators.
char numberBuffer[BUFFER_SIZE];

// ******** Private methods ********

/// <summary>
/// Fast division and modulo by 10 for unsigned long integers.
/// </summary>
/// <param name="in">Number to divide.</param>
/// <param name="pDivResult">Pointer to store the division result.</param>
/// <param name="pModResult">Pointer to store the modulo result.</param>
static inline void divmod10f32(uint16_t const in, uint16_t* const pDivResult, uint16_t* const pModResult) {
   uint16_t x = (in | 1) - (in >> 2);
   uint16_t q = (x >> 4) + x;

   x = q;
   q = (q >> 8) + x;
   q = (q >> 8) + x;

   uint16_t divResult = (q >> 3);
   *pDivResult = divResult;
   *pModResult = in - ((q & ~7) + (divResult << 1));
}

// ******** Public methods ********

/// <summary>
/// Print an unsigned 16-bit integer with thousands groups.
/// </summary>
/// <param name="aNumber">Number to proint.</param>
/// <param name="separator">Thousands group separator character.</param>
/// <returns>Pointer to string of number.</returns>
char* FormatUint16WithSeparator(uint16_t const aNumber, char const separator) {
   uint16_t last = aNumber;
   char* pBuffer = &numberBuffer[BUFFER_SIZE - 1];  // Start at the end of the buffer.
   *pBuffer-- = 0;  // Set terminating zero.

   uint8_t groupCount = 0;
   do {
      if (groupCount >= 3) {
         *pBuffer-- = separator;
         groupCount = 0;
      }

      uint16_t divResult;
      uint16_t modResult;
      divmod10f32(last, &divResult, &modResult);

      *pBuffer-- = (char)modResult + '0';
      groupCount++;
      last = divResult;
   } while (last != 0);

   return ++pBuffer;
}

/// <summary>
/// Print an unsigned 16-bit integer with thousands groups separated by the default separator character.
/// </summary>
/// <param name="aNumber">Number to proint.</param>
/// <returns>Pointer to string of number.</returns>
char* FormatUint16NumberWithDefaultSeparator(uint16_t const aNumber) {
   return FormatUint16WithSeparator(aNumber, DEFAULT_SEPARATOR);
}

/// <summary>
/// Print an unsigned 16-bit integer.
/// </summary>
/// <param name="aNumber">Number to proint.</param>
/// <returns>Pointer to string of number.</returns>
char* FormatUint16Number(uint16_t const aNumber) {
   uint16_t last = aNumber;
   char* pBuffer = &numberBuffer[BUFFER_SIZE - 1];  // Start at the end of the buffer.
   *pBuffer-- = 0;  // Set terminating zero.

   do {
      uint16_t divResult;
      uint16_t modResult;
      divmod10f32(last, &divResult, &modResult);

      *pBuffer-- = (char)modResult + '0';
      last = divResult;
   } while (last != 0);

   return ++pBuffer;
}
