#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

#include "ApiErrorHandler.h"
#include "PrintModVersion.h"

void printListType(ULONG listType) {
	_putws(L"");

	switch (listType) {
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
	}
}

void ListForType(ULONG listType) {
	printListType(listType);
	ULONG algoCount;
	BCRYPT_ALGORITHM_IDENTIFIER* pAlgoList;
	NTSTATUS nts = BCryptEnumAlgorithms(listType, &algoCount, &pAlgoList, 0);
	if (nts < 0) {
		PrintNtStatus(L"ListForType", L"BCryptEnumAlgorithms", nts);
		return;
	}

	BCRYPT_ALGORITHM_IDENTIFIER* pActAlgo = pAlgoList;
	for (ULONG i = 0; i < algoCount; i++) {
		wprintf(L"   %s\n", pActAlgo->pszName);
		pActAlgo++;
	}

	BCryptFreeBuffer(pAlgoList);

	_putws(L"");
}

void ListAllTypes() {
	fputws(L"\nList of Bcrypt ", stdout);
	PrintModuleVersion(L"bcrypt.dll");
	_putws(L" algorithms by type : ");

	ListForType(BCRYPT_CIPHER_OPERATION);
	ListForType(BCRYPT_HASH_OPERATION);
	ListForType(BCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION);
	ListForType(BCRYPT_SECRET_AGREEMENT_OPERATION);
	ListForType(BCRYPT_SIGNATURE_OPERATION);
	ListForType(BCRYPT_RNG_OPERATION);
}
