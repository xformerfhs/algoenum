#pragma once

#include <Windows.h>

/// <summary>
/// Sort a list of LPWSTR pointers.
/// </summary>
/// <param name="pList">List to sort.</param>
/// <param name="count">Number of elements in the list.</param>
void SortList(LPWSTR const* pList, const ULONG count);