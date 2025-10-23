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
// Version: 1.4.0
//
// Change history:
//    2023-12-01: V1.0.0: Created.
//    2024-12-14: V1.1.0: Sorted algorithm name lists.
//    2025-10-21: V1.2.0: Corrected error message, if heap allocation failed.
//                        Get output file pointer only once.
//    2025-10-21: V1.2.1: Simplified header output.
//    2025-10-21: V1.3.0: Use shellsort, instead of quicksort.
//    2025-10-22: V1.4.0: List all types and collect overall result.
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
/// Sort the list of algorithm names with the shell sort algorithm.
/// </summary>
/// <param name="pAlgorithmNames">Pointer to list of algorithm names.</param>
/// <param name="algorithmCount">Count of algorithm names.</param>
static void shellSort(LPWSTR* const pAlgorithmNames, const USHORT algorithmCount) {
	USHORT stepSize[] = {7, 4, 1};

	for (USHORT s = 0; s < sizeof(stepSize) / sizeof(stepSize[0]); s++) {
		USHORT step = stepSize[s];

		for (USHORT i = step; i < algorithmCount; i++) {
			LPWSTR insertionName = pAlgorithmNames[i];
			USHORT insertionIndex = i;
			
			while (insertionIndex >= step &&
					 wcscmp(insertionName, pAlgorithmNames[insertionIndex - step]) < 0) {
				pAlgorithmNames[insertionIndex] = pAlgorithmNames[insertionIndex - step];
            insertionIndex -= step;
			}

			if (insertionIndex != i)
				pAlgorithmNames[insertionIndex] = insertionName;
		}
   }
}

/// <summary>
/// Print the type of the elements in the list.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
/// <param name="fStdOut">Stdout file pointer.</param>
static void printAlgorithmTypeName(const ULONG algorithmType, FILE* fStdOut) {
   _putwc_nolock(L'\n', fStdOut);

	switch (algorithmType) {
	case BCRYPT_CIPHER_OPERATION:
		fputws(L"Symmetric ciphers", fStdOut);
		break;

	case BCRYPT_HASH_OPERATION:
		fputws(L"Hashes", fStdOut);
		break;

	case BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION:
		fputws(L"Asymmetric ciphers", fStdOut);
		break;

	case BCRYPT_SECRET_AGREEMENT_OPERATION:
		fputws(L"Secret agreements", fStdOut);
		break;

	case BCRYPT_SIGNATURE_OPERATION:
		fputws(L"Signatures", fStdOut);
		break;

	case BCRYPT_RNG_OPERATION:
		fputws(L"Pseudorandom Number Generators", fStdOut);
		break;

	case BCRYPT_KEY_DERIVATION_OPERATION:
		fputws(L"Key derivation", fStdOut);
		break;

	default:
		fwprintf(fStdOut, L"Unknown algorithm type 0x%x", algorithmType);
	}

   fputws(L":\n\n", fStdOut);
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
/// Print the list of algorithm names for the specified type.
/// </summary>
/// <param name="listType">BCrypt algorithm type.</param>
/// <param name="fStdOut">Stdout file pointer.</param>
static BOOL listForType(const HANDLE hHeap, const ULONG algorithmType, FILE* fStdOut) {
	const PWCHAR functionName = L"listForType";

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
   shellSort(pSortedList, (USHORT)algoCount);

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

   FILE* fStdOut = stdout;

	// 1. Print header.
	fputws(L"\nList of Bcrypt ", fStdOut);
	PrintModuleVersion(L"bcrypt.dll", fStdOut);
	fputws(L" algorithms by type:\n\n", fStdOut);
	
	// 2. Get the process heap to use in the list functions.
	
	// Process heap.
	HANDLE hHeap = GetProcessHeap();
	if (hHeap == NULL) {
		PrintLastError(functionName, L"GetProcessHeap");
		return RC_ERR;
	}

	// 3. Print lists for each type.
	BOOL result = listForType(hHeap, BCRYPT_CIPHER_OPERATION, fStdOut);
	result &= listForType(hHeap, BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION, fStdOut);
	result &= listForType(hHeap, BCRYPT_HASH_OPERATION, fStdOut);
	result &= listForType(hHeap, BCRYPT_SECRET_AGREEMENT_OPERATION, fStdOut);
	result &= listForType(hHeap, BCRYPT_SIGNATURE_OPERATION, fStdOut);
	result &= listForType(hHeap, BCRYPT_RNG_OPERATION, fStdOut);
	result &= listForType(hHeap, BCRYPT_KEY_DERIVATION_OPERATION, fStdOut);

   if (result == FALSE)
		return RC_ERR;

	return RC_OK;
}
