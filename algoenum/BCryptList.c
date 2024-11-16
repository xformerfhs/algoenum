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
// Version: 1.1.0
//
// Change history:
//    2023-12-01: V1.0.0: Created.
//    2024-12-14: V1.1.0: Sort algorithm name lists.
//

#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

#include "ApiErrorHandler.h"
#include "PrintModVersion.h"

// ======== Private constants ========

#define RC_OK 0
#define RC_ERR 0xff

// ======== Private methods ========

/// <summary>
/// Print the type of the elements in the list.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
void printAlgorithmTypeName(const ULONG algorithmType) {
	_putws(L"");

	switch (algorithmType) {
	case BCRYPT_CIPHER_OPERATION:
		_putws(L"Symmetric ciphers:\n");
		break;

	case BCRYPT_HASH_OPERATION:
		_putws(L"Hashes:\n");
		break;

	case BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION:
		_putws(L"Asymmetric ciphers:\n");
		break;

	case BCRYPT_SECRET_AGREEMENT_OPERATION:
		_putws(L"Secret agreements:\n");
		break;

	case BCRYPT_SIGNATURE_OPERATION:
		_putws(L"Signatures:\n");
		break;

	case BCRYPT_RNG_OPERATION:
		_putws(L"Pseudorandom Number Generators:\n");
		break;

	default:
		wprintf(L"Unknown algorithm type: 0x%x\n", algorithmType);
	}
}

/// <summary>
/// Copy the pointer to the algorithm names from the BCrypt algorithm list into a local memory area.
/// </summary>
/// <param name="hHeap">Handle of the local heap.</param>
/// <param name="pAlgoList">Pointer to the list of BCrypt algorithm identifiers.</param>
/// <param name="algoCount">Number of algorithms.</param>
/// <returns>Pointer to the local copy of the algorithm name pointers.</returns>
LPWSTR* copyAlgorithmNames(const HANDLE hHeap, BCRYPT_ALGORITHM_IDENTIFIER* pAlgoList, const ULONG algoCount) {
	WCHAR const* functionName = L"copyAlgorithmNames";

	LPWSTR* pSortedList = HeapAlloc(hHeap, 0, algoCount * sizeof(LPWSTR));
	if (pSortedList == NULL) {
		PrintLastError(functionName, L"HeapAlloc");
		return pSortedList;
	}

	// Pointer to algorithm identifier.
	BCRYPT_ALGORITHM_IDENTIFIER* pActAlgo = pAlgoList;
	LPWSTR* pName = pSortedList;
	for (ULONG i = algoCount; i > 0; i--) {
		*pName = pActAlgo->pszName;
		pActAlgo++;
		pName++;
	}

	return pSortedList;
}


/// <summary>
/// Comparison function for qsort.
/// </summary>
/// <param name="a">Pointer to one element.</param>
/// <param name="b">Pointer to other element.</param>
/// <returns>Negative, if *a is less than *b, 0, if *a == *b and positive, if *a greater than *b.</returns>
int compareNames(const void* a, const void* b) {
	return wcscmp(*(LPWSTR const*)a, *(LPWSTR const*)b);
}

/// <summary>
/// Print the list of algorithm names for the specified type.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
BOOL listForType(const HANDLE hHeap, const ULONG algorithmType) {
	WCHAR const* functionName = L"listForType";

	// 1. Print the algorithm type.
	printAlgorithmTypeName(algorithmType);

	// 2. Get the list of algorithms of this type.
	ULONG algoCount;
	BCRYPT_ALGORITHM_IDENTIFIER* pAlgoList;
	NTSTATUS nts = BCryptEnumAlgorithms(algorithmType, &algoCount, &pAlgoList, 0);
	if (nts < 0) {
		PrintNtStatus(functionName, L"BCryptEnumAlgorithms", nts);
		return FALSE;
	}

	// 3. Sort the algorithm names.

	// 3.1 Copy the pointers to the names into a local memory area.

	// Pointer to list of string pointers to algorithm names.
	LPWSTR* pSortedList = copyAlgorithmNames(hHeap, pAlgoList, algoCount);
	if (pSortedList == NULL)
		return FALSE;

	// 3.2 Sort the string pointers in the list.
	qsort(pSortedList, algoCount, sizeof(LPWSTR), compareNames);

	// 4. Print the sorted list of names.

	// Pointer to algorithm identifier.
	LPWSTR* pActAlgoName = pSortedList;
	for (ULONG i = algoCount; i > 0; i--) {
		fputws(L"   ", stdout);
		_putws(*pActAlgoName);
		pActAlgoName++;
	}

	// 5. Release memory.
	HeapFree(hHeap, 0, pSortedList);
	BCryptFreeBuffer(pAlgoList);

	// 6. Add a new line.
	_putws(L"");

	return TRUE;
}

// ======== Public methods ========

/// <summary>
/// Print the names of all BCrypt algorithms.
/// </summary>
unsigned char ListAllTypes() {
	WCHAR const* functionName = L"ListAllTypes";

	// 1. Print header.
	fputws(L"\nList of Bcrypt ", stdout);
	PrintModuleVersion(L"bcrypt.dll");
	_putws(L" algorithms by type:");
	
	// 2. Get the process heap to use in the list functions.
	
	// Process heap.
	HANDLE hHeap = GetProcessHeap();
	if (hHeap == NULL) {
		PrintLastError(functionName, L"GetProcessHeap");
		return RC_ERR;
	}

	// 3. Print lists for each type.
	if (listForType(hHeap, BCRYPT_CIPHER_OPERATION) == FALSE)
		return RC_ERR;
	if (listForType(hHeap, BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION) == FALSE)
		return RC_ERR;
	if (listForType(hHeap, BCRYPT_HASH_OPERATION) == FALSE)
		return RC_ERR;
	if (listForType(hHeap, BCRYPT_SECRET_AGREEMENT_OPERATION) == FALSE)
		return RC_ERR;
	if (listForType(hHeap, BCRYPT_SIGNATURE_OPERATION) == FALSE)
		return RC_ERR;
	if (listForType(hHeap, BCRYPT_RNG_OPERATION) == FALSE)
		return RC_ERR;

	return RC_OK;
}
