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
// Author: Frank Schwab
//
// Version: 1.0.0
//
// Change history:
//    2025-11-12: V1.0.0: Created.
//

#include <Windows.h>
#include <stdio.h>

// ******** Private constants ********

/// Length of message buffers.
#define MESSAGE_BUFFER_LENGTH 256

// ******** Private variables ********

/// Buffer for wide character message text.
static wchar_t wideBuffer[MESSAGE_BUFFER_LENGTH];

/// Buffer for console character message text.
static char consoleBuffer[MESSAGE_BUFFER_LENGTH];

/// Code page of the console.
UINT consoleCodePage = 0;

// ******** Public functions ********

/// <summary>
/// Convert a wide character string into a string for the console code page.
/// </summary>
/// <param name="wideMessage">UTF-16 message.</param>
/// <returns>Pointer to message converted to the console code page.</returns>
char* AsConsoleCodePageString(const wchar_t* const wideMessage) {
   if (consoleCodePage == 0)
      consoleCodePage = GetConsoleOutputCP();

   int len = WideCharToMultiByte(
      consoleCodePage,        // Destination code page.
      0,                      // No special flags.
      wideMessage,            // UTF-16 source string.
      -1,                     // Source string is 0 terminated.
      consoleBuffer,          // Destination buffer.
      MESSAGE_BUFFER_LENGTH,  // Length of destination buffer.
      NULL,                   // Use system default char for unmappable chars.
      NULL                    // Not interested in whether default char was used.
   );

   if (len == 0) {
      DWORD le = GetLastError();
      sprintf_s(
         consoleBuffer,
         MESSAGE_BUFFER_LENGTH,
         "Error converting message to console code page: %lu (0x%08lx)",
         le,
         le
      );
   }

   return consoleBuffer;
}

/// <summary>
/// Prints a wide character formatted string to the console.
/// </summary>
/// <param name="stream">Output stream.</param>
/// <param name="format">Format.</param>
/// <param name="">Parameters for format.</param>
void PrintWideFormatToConsole(FILE* const stream, const wchar_t* const format, ...) {
   va_list args;
   va_start(args, format);
   vswprintf(wideBuffer, MESSAGE_BUFFER_LENGTH, format, args);
   va_end(args);
   fputs(AsConsoleCodePageString(wideBuffer), stream);
}
