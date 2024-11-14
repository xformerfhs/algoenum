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
// Purpose: Main program for crypto benchmark.
// 
// Author: Frank Schwab
//
// Version: 1.0.1
//
// Change history:
//    2024-06-01: V1.0.0: Created.
//    2024-11-14: V1.0.1: Return code depends on result of printing the lists.
//

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "BCryptList.h"

// ======== Private constants ========

#define RC_OK 0
#define RC_CMD_ERR 1
#define RC_PROC_ERR 2

// ======== Main method ========

int __cdecl wmain(int const argc, wchar_t const* argv[]) {
	(void)_setmode(_fileno(stdout), _O_U16TEXT);  // So the console (stdout) is able to display unicode characters
	(void)_setmode(_fileno(stderr), _O_U16TEXT);  // So the console (stderr) is able to display unicode characters

	if (ListAllTypes() == 0)
		return RC_OK;
	else
		return RC_PROC_ERR;
}
