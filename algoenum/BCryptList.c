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
// Version: 1.2.0
//
// Change history:
//    2023-12-01: V1.0.0: Created.
//    2024-12-14: V1.1.0: Sorted algorithm name lists.
//    2025-10-21: V1.2.0: Corrected error message, if heap allocation failed.
//                        Get output file pointer only once.
//

#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

#include "ApiErrorHandler.h"
#include "PrintModVersion.h"


// ******** Private constants ********

#define RC_OK  0
#define RC_ERR 0xff


// ******** Private methods ********

/// <summary>
/// Print the type of the elements in the list.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
static void printAlgorithmTypeName(const ULONG algorithmType, FILE* fStdOut) {
   _putwc_nolock(L'\n', fStdOut);

	switch (algorithmType) {
	case BCRYPT_CIPHER_OPERATION:
		fputws(L"Symmetric ciphers:\n", fStdOut);
		break;

	case BCRYPT_HASH_OPERATION:
		fputws(L"Hashes:\n", fStdOut);
		break;

	case BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION:
		fputws(L"Asymmetric ciphers:\n", fStdOut);
		break;

	case BCRYPT_SECRET_AGREEMENT_OPERATION:
		fputws(L"Secret agreements:\n", fStdOut);
		break;

	case BCRYPT_SIGNATURE_OPERATION:
		fputws(L"Signatures:\n", fStdOut);
		break;

	case BCRYPT_RNG_OPERATION:
		fputws(L"Pseudorandom Number Generators:\n", fStdOut);
		break;

	default:
		fwprintf(fStdOut, L"Unknown algorithm type: 0x%x\n", algorithmType);
	}

	_putwc_nolock(L'\n', fStdOut);
}

/// <summary>
/// Copy the pointers to the algorithm names from the BCrypt algorithm list into a local memory area.
/// </summary>
/// <param name="hHeap">Handle of the local heap.</param>
/// <param name="pAlgoList">Pointer to the list of BCrypt algorithm identifiers.</param>
/// <param name="algoCount">Number of algorithms.</param>
/// <returns>Pointer to the local copy of the algorithm name pointers.</returns>
static LPWSTR* copyAlgorithmNamePointers(const HANDLE hHeap, BCRYPT_ALGORITHM_IDENTIFIER* const pAlgoList, const ULONG algoCount) {
	const PWCHAR functionName = L"copyAlgorithmNames";

	LPWSTR* pNameList = HeapAlloc(hHeap, 0, algoCount * sizeof(LPWSTR));
	if (pNameList == NULL) {
      (void) fwprintf(stderr, L"Function \"%s\": HeapAlloc for algorithm name list failed.\n", functionName);
		return pNameList;
	}

	// Pointer to algorithm identifier.
	BCRYPT_ALGORITHM_IDENTIFIER* pActAlgo = pAlgoList;
	LPWSTR* pName = pNameList;
	for (ULONG i = algoCount; i > 0; i--)
		*pName++ = pActAlgo++->pszName;

	return pNameList;
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
static BOOL listForType(const HANDLE hHeap, const ULONG algorithmType) {
	const PWCHAR functionName = L"listForType";

	FILE* fStdOut = stdout;

	// 1. Print the algorithm type.
	printAlgorithmTypeName(algorithmType, fStdOut);

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
	//     This is necessary, so that this list can be sorted with qsort.
	
	// Pointer to list of string pointers to algorithm names.
	LPWSTR* pSortedList = copyAlgorithmNamePointers(hHeap, pAlgoList, algoCount);
	if (pSortedList == NULL) {
		BCryptFreeBuffer(pAlgoList);
		return FALSE;
	}

	// 3.2 Sort the string pointers in the list.
	qsort(pSortedList, algoCount, sizeof(LPWSTR), compareNames);

	// 4. Print the sorted list of names.

	// Pointer to algorithm identifier.
	LPWSTR* pActAlgoName = pSortedList;
	for (ULONG i = algoCount; i > 0; i--) {
		fputws(L"   ", fStdOut);
		fputws(*pActAlgoName++, fStdOut);
		_putwc_nolock(L'\n', fStdOut);
	}

	// 5. Add a new line at the end of the list.
	_putwc_nolock(L'\n', fStdOut);

	// 6. Release memory.
   HeapFree(hHeap, 0, pSortedList);  // This must be freed *before* the algorithm list is freed.
	BCryptFreeBuffer(pAlgoList);      // This must be freed *after* the names have been printed.

	return TRUE;
}


// ******** Public methods ********

/// <summary>
/// Print the names of all BCrypt algorithms.
/// </summary>
unsigned char ListAllTypes() {
	const PWCHAR functionName = L"ListAllTypes";

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
