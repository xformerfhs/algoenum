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
//    2023-12-01: V1.0.0: Created.
//

#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

#include "ApiErrorHandler.h"
#include "PrintModVersion.h"

// ======== Private methods ========

/// <summary>
/// Print the type of the elements in the list.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
void printAlgorithmTypeName(const ULONG algorithmType) {
	_putws(L"");

	switch (algorithmType) {
	case BCRYPT_CIPHER_OPERATION:
		_putws(L"Ciphers:\n");
		break;

	case BCRYPT_HASH_OPERATION:
		_putws(L"Hashes:\n");
		break;

	case BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION:
		_putws(L"Asymmetric operations:\n");
		break;

	case BCRYPT_SECRET_AGREEMENT_OPERATION:
		_putws(L"Secret agreements:\n");
		break;

	case BCRYPT_SIGNATURE_OPERATION:
		_putws(L"Signature operations:\n");
		break;

	case BCRYPT_RNG_OPERATION:
		_putws(L"PRNG algoritmhs:\n");
		break;

	default:
		wprintf(L"Unknown algorithm type: 0x%x\n", algorithmType);
	}
}

/// <summary>
/// Print the list of algorithm names for the specified type.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
void listForType(const ULONG algorithmType) {
	printAlgorithmTypeName(algorithmType);
	
	ULONG algoCount;
	BCRYPT_ALGORITHM_IDENTIFIER* pAlgoList;
	NTSTATUS nts = BCryptEnumAlgorithms(algorithmType, &algoCount, &pAlgoList, 0);
	if (nts < 0) {
		PrintNtStatus(L"ListForType", L"BCryptEnumAlgorithms", nts);
		return;
	}

	BCRYPT_ALGORITHM_IDENTIFIER* pActAlgo = pAlgoList;
	for (ULONG i = 0; i < algoCount; i++) {
		fputws(L"   ", stdout);
		_putws(pActAlgo->pszName);
		pActAlgo++;
	}

	BCryptFreeBuffer(pAlgoList);

	_putws(L"");
}

// ======== Public methods ========

/// <summary>
/// Print the names of all BCrypt algorithms.
/// </summary>
void ListAllTypes() {
	fputws(L"\nList of Bcrypt ", stdout);
	PrintModuleVersion(L"bcrypt.dll");
	_putws(L" algorithms by type : ");

	listForType(BCRYPT_CIPHER_OPERATION);
	listForType(BCRYPT_HASH_OPERATION);
	listForType(BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION);
	listForType(BCRYPT_SECRET_AGREEMENT_OPERATION);
	listForType(BCRYPT_SIGNATURE_OPERATION);
	listForType(BCRYPT_RNG_OPERATION);
}
