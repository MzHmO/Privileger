#include "privileger.h"

//Dialog Func
VOID ShowHelp() {
	std::wcout << L"Usage: \n\n Mode 1 (Adding Privileges to account) \n\t Privileger.exe 1 <account name> <privilege> \n \tEx: Privileger.exe 1 Michael SeDebugPrivilege" << std::endl;
	std::wcout << L"\n\n Mode 2 (Start process with privilege) \n\t Privileger.exe 2 <path to exe> <privilege> \n \tEx: Privileger.exe 2 C:\\Windows\\System32\\cmd.exe SeDebugPrivilege" << std::endl;
	std::wcout << L"\n\n Mode 3 (Remove Privileges from account) \n\t Privileger.exe 3 <account name> <privilege> \n \tEx: Privileger.exe 3 Michael SeDebugPrivilege" << std::endl;
}

VOID ShowAwesomeBanner() {
	std::cout << R"(
  _____      _       _ _                      
 |  __ \    (_)     (_) |                     
 | |__) | __ ___   ___| | ___  __ _  ___ _ __ 
 |  ___/ '__| \ \ / / | |/ _ \/ _` |/ _ \ '__|
 | |   | |  | |\ V /| | |  __/ (_| |  __/ |   
 |_|   |_|  |_| \_/ |_|_|\___|\__, |\___|_|   
                               __/ |          
                              |___/           v 1.0)" << std::endl;
	std::wcout << L"\n\n\t\t\t https://github.com/MzHmO" << std::endl;
}

DWORD InitMode1(wchar_t* cAccName, wchar_t* cPrivName) {
	std::wcout << L"[+] Initializing mode 1 \n [+] Target Account: " << cAccName << "\n [+] Privilege: " << cPrivName << std::endl;
	LSA_HANDLE hPolicy;
	if (GetPolicy(&hPolicy) != 0) {
		std::wcout << L" [-] GetPolicy() Error: " << std::endl;
		return 1;
	}
	AddUserPrivilege(hPolicy, cAccName, cPrivName, TRUE);
	return 0;
}

DWORD InitMode2(wchar_t* cPath, wchar_t* cPrivName) {
	std::wcout << L"[+] Initializing mode 2 \n [+] Path to exe: " << cPath << "\n [+] Privilege: " << cPrivName << std::endl;
	
	ImpersonateSelf(SecurityImpersonation);
	HANDLE hToken = NULL;
	OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, FALSE, &hToken);
	DWORD dw = ::GetLastError();
	if (dw != 0) {
		std::wcout << L"[!] Error OpenThreadToken(): " << dw << std::endl;
		return 1;
	}

	if (EnableTokenPrivilege(hToken, cPrivName, TRUE) == 0) {
		std::wcout << L" [+] EnableTokenPrivilege() success" << std::endl;
		STARTUPINFO startupInfo;
		ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
		PROCESS_INFORMATION processInformation;
		ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
		startupInfo.cb = sizeof(STARTUPINFO);
		CreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE, NULL, cPath, 0, NULL, NULL, &startupInfo, &processInformation);
		DWORD dw = ::GetLastError();
		if (dw != 0) {
			std::wcout << L"[!] Error CreateProcessWithTokenW(): " << dw << std::endl;
			return 1;
		}
		else {
			std::wcout << L"[+] CreateProcessWithTokenW() success" << std::endl;
			return 0;
		}

	}
	else {
		std::wcout << L" [-] EnableTokenPrivilege() failed" << std::endl;
		return 1;
	}

	return 0;
}

DWORD InitMode3(wchar_t* cAccName, wchar_t* cPrivName) {
	std::wcout << L"[+] Initializing mode 1 \n [+] Target Account: " << cAccName << "\n [+] Privilege: " << cPrivName << std::endl;
	LSA_HANDLE hPolicy;
	if (GetPolicy(&hPolicy) != 0) {
		std::wcout << L" [-] GetPolicy() Error: " << std::endl;
		return 1;
	}
	AddUserPrivilege(hPolicy, cAccName, cPrivName, FALSE);
	return 0;
}

// Prod Func

DWORD AddUserPrivilege(LSA_HANDLE hPolicy, LPWSTR wUsername, LPWSTR wPrivName, BOOL bEnable) {
	DWORD sid_size = 0;
	PSID UserSid;
	LPTSTR wSidStr = NULL;
	DWORD domain_size = 0;
	SID_NAME_USE sid_use;
	if (!LookupAccountName(NULL, wUsername, NULL, &sid_size, NULL, &domain_size, &sid_use)) {
		UserSid = (PSID)VirtualAlloc(NULL, sid_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		LPTSTR domain = NULL;
		domain = (LPTSTR)VirtualAlloc(NULL, domain_size * sizeof(WCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		LookupAccountName(NULL, wUsername, UserSid, &sid_size, domain, &domain_size, &sid_use);
		VirtualFree(domain, 0, MEM_RELEASE);
		ConvertSidToStringSid(UserSid, &wSidStr);
		std::wcout << L" [+] User SID: " << wSidStr << std::endl;

		LSA_UNICODE_STRING lsastrPrivs[1] = { 0 };

		lsastrPrivs[0].Buffer = (PWSTR)wPrivName;
		lsastrPrivs[0].Length = lstrlen(lsastrPrivs[0].Buffer) * sizeof(WCHAR);
		lsastrPrivs[0].MaximumLength = lsastrPrivs[0].Length + sizeof(WCHAR);

		if (bEnable) {
			NTSTATUS ntStatus = LsaAddAccountRights(hPolicy, UserSid, lsastrPrivs, 1);

			ULONG lErr = LsaNtStatusToWinError(ntStatus);
			if (lErr == ERROR_SUCCESS) {
				std::wcout << L" [+] Adding " << wPrivName << L" Success" << std::endl;

				std::wcout << L" [+] Enumerating Current Privs" << std::endl;
				PrintTrusteePrivs(hPolicy, UserSid);
				VirtualFree(UserSid, 0, MEM_RELEASE);
				return 0;
			}
			else {
				wprintf(L" [-] Error LsaAddAccountRights() %d", lErr);
				return 1;
			}
		} else {
			ULONG lErr = LsaRemoveAccountRights(hPolicy, UserSid, FALSE, lsastrPrivs, 1);
			if (lErr == ERROR_SUCCESS) {
				std::wcout << L" [-] Removing " << wPrivName << L" Success" << std::endl;
				std::wcout << L" [+] Enumerating Current Privs" << std::endl;
				PrintTrusteePrivs(hPolicy, UserSid);
				VirtualFree(UserSid, 0, MEM_RELEASE);
				return 0;
			}
			else {
				wprintf(L" [-] Error LsaRemoveAccountRights() %d", lErr);
				return 1;
			}
		}
	}
	else {
		std::wcout << L" [-] LookupAccountName() Error: " << GetLastError() << std::endl;
		return 1;
	}
	return 1;
}

DWORD GetPolicy(PLSA_HANDLE LsaHandle){
	wchar_t cCompName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
	DWORD size = sizeof(cCompName);
	if (GetComputerNameW(cCompName, &size)) {
		std::wcout << L" [+] ComputerName: " << cCompName << std::endl;
	}
	else {
		std::wcout << L" [-] GetComputerNameW Error: " << GetLastError() << std::endl;
	}

	LSA_OBJECT_ATTRIBUTES lsaOA = { 0 };
	LSA_UNICODE_STRING lsastrComputer = { 0 };
	lsaOA.Length = sizeof(lsaOA);
	lsastrComputer.Length = (USHORT)(lstrlen(cCompName) * sizeof(WCHAR));
	lsastrComputer.MaximumLength = lsastrComputer.Length + sizeof(WCHAR);
	lsastrComputer.Buffer = (PWSTR)&cCompName;
	NTSTATUS ntStatus = LsaOpenPolicy(&lsastrComputer, &lsaOA, POLICY_ALL_ACCESS, LsaHandle);

	ULONG lErr = LsaNtStatusToWinError(ntStatus);
	if (lErr != ERROR_SUCCESS) {
		std::wcout << L" [-] LsaOpenPolicy() Error: " << lErr << std::endl;
		return 1;
	}
	else {
		std::wcout << L" [+] LsaOpenPolicy() Success" << std::endl;
		return 0;
	}
	return 1;
}

DWORD PrintTrusteePrivs(LSA_HANDLE hPolicy, PSID psid) {
	BOOL fSuccess = FALSE;
	WCHAR szTempPrivBuf[256];
	WCHAR szPrivDispBuf[1024];
	PLSA_UNICODE_STRING plsastrPrivs = NULL;

	__try {
		ULONG lCount = 0;
		NTSTATUS ntStatus = LsaEnumerateAccountRights(hPolicy, psid, &plsastrPrivs, &lCount);
		ULONG lErr = LsaNtStatusToWinError(ntStatus);
		if (lErr != ERROR_SUCCESS) {
			plsastrPrivs = NULL;
			__leave;
		}
		ULONG lDispLen = 0;
		ULONG lDispLang = 0;
		for (ULONG lIndex = 0; lIndex < lCount; lIndex++) {
			lstrcpyn(szTempPrivBuf, plsastrPrivs[lIndex].Buffer, plsastrPrivs[lIndex].Length);
			szTempPrivBuf[plsastrPrivs[lIndex].Length] = 0;

			wprintf(L"\tProgrammatic name: %s\n", szTempPrivBuf);

			lDispLen = 1024;
			if (LookupPrivilegeDisplayNameW(NULL, szTempPrivBuf, szPrivDispBuf, &lDispLen, &lDispLang))
				wprintf(L"\tDisplay Name: %ws\n\n", szPrivDispBuf);
		}
		fSuccess = TRUE;
	}
	__finally {
		if (plsastrPrivs) LsaFreeMemory(plsastrPrivs);
	}
	return (fSuccess);
}

DWORD EnableTokenPrivilege(HANDLE hToken, LPTSTR szPriv, BOOL bEnabled) {
	TOKEN_PRIVILEGES tp;
	LUID luid;
	if (!LookupPrivilegeValue(NULL, szPriv, &luid)) {
		std::wcout << L"[-] LookupPrivilegeValue() Error: " << GetLastError() << std::endl;
		return 1;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = bEnabled ? SE_PRIVILEGE_ENABLED : 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		std::wcout << L"[-] AdjustTokenPrivileges() Error: " << GetLastError() << std::endl;
		return 1;
	}
	return 0;
}

DWORD ValidateAccInfo(wchar_t* cAccName, wchar_t* cPrivName) {
	// validating username
	DWORD sid_size = 0;
	PSID UserSid;
	LPTSTR wSidStr = NULL;
	DWORD domain_size = 0;
	SID_NAME_USE sid_use;
	DWORD wow = LookupAccountName(NULL, cAccName, NULL, &sid_size, NULL, &domain_size, &sid_use);
	DWORD dw = GetLastError();
	if ((wow == 0) && ( (dw == 122) || (dw == 0))) {
		std::wcout << L"[+] User " << cAccName << L" found" << std::endl;

		// validating Privilege name
		LUID luid;
		if (!LookupPrivilegeValue(NULL, cPrivName, &luid)) {
			std::wcout << L"[-] Privilege " << cPrivName << L" may be incorrect" << std::endl;
		}
		else {
			std::wcout << L"[+] Privilege " << cPrivName << L" Found \n[+] Validation Success" << std::endl;
			return 0;
		}
	}
	else {
		std::wcout << L"[-] Username may be incorrect. LookupAccountName() Err: " << dw << std::endl;
		return 1;
	}
	return 1;
}

DWORD ValidatePathInfo(wchar_t* Path, wchar_t* cPrivName) {
	BOOL bPathEx = PathFileExistsW(Path);
	if (bPathEx) {
		std::wcout << L"[+] " << Path << L" Found" << std::endl;
		LUID luid;
		if (!LookupPrivilegeValue(NULL, cPrivName, &luid)) {
			std::wcout << L"[-] Privilege " << cPrivName << L" may be incorrect" << std::endl;
			return 1;
		}
		else {
			std::wcout << L"[+] Privilege " << cPrivName << L" Found \n[+] Validation Success" << std::endl;
			return 0;
		}
		return 0;
	}
	else {
		std::wcout << L"[-] " << Path << L" not found" << std::endl;
		return 1;
	}
	return 1;
}

int wmain(int argc, wchar_t* argv[]) {
	setlocale(LC_ALL, "");
	ShowAwesomeBanner();

	DWORD dwRC = 0, dwV = 0;
	if (argc != 4) {
		ShowHelp();
		return 0;
	}
	

	switch (*argv[1]) {
	case '1':
		if (ValidateAccInfo(argv[2], argv[3]) == 0) {
			dwRC = InitMode1(argv[2], argv[3]);
		}
		else {
			std::wcout << L"[-] ValidateAccInfo() Failed" << std::endl;
		}
		break;
	case '2':
		if (ValidatePathInfo(argv[2], argv[3]) == 0) {
			dwRC = InitMode2(argv[2], argv[3]);
		}
		else {
			std::wcout << L"[-] ValidatePathInfo() Failed" << std::endl;
		}
		break;
	case '3':
		if (ValidateAccInfo(argv[2], argv[3]) == 0) {
			dwRC = InitMode3(argv[2], argv[3]);
		}
		else {
			std::wcout << L"[-] ValidateAccInfo() Failed" << std::endl;
		}
		break;
	default:
		std::wcout << L"[-] No such mode" << std::endl;
		return 0;
	}
	return dwRC;
}