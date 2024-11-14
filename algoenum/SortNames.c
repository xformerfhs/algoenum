//
// SPDX-FileCopyrightText: Copyright 2024 Frank Schwab
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
//    2024-11-14: V1.0.0: Created.
//

#include <Windows.h>

/// <summary>
/// Sort a list of LPWSTR pointers.
/// </summary>
/// <param name="pList">List to sort.</param>
/// <param name="count">Number of elements in the list.</param>
void SortList(LPWSTR* pList, const ULONG count) {
   // We need at least 2 elements to sort.
   if (count < 2)
      return;

   // This is a simple insertion sort.
   // It is not particularly efficient, but that is not necessary for the short lists
   // we are handling here.

   LPWSTR* pToSort = pList + 1;
   for (ULONG i = count - 1; i > 0; i--) {
      LPWSTR pSortValue = *pToSort;

      LPWSTR* pBelowPlus1 = pToSort;
      LPWSTR* pBelow = pToSort - 1;
      for (ULONG j = count - i; j > 0; j--) {
         LPWSTR pCompareValue = *pBelow;
         if (wcscmp(pCompareValue, pSortValue) > 0) {
            *pBelowPlus1 = pCompareValue;
            pBelowPlus1 = pBelow;
            pBelow--;
         } else
            break;
      }
   
      if (pBelowPlus1 != pToSort)
         *pBelowPlus1 = pSortValue;

      pToSort++;
   }
}
