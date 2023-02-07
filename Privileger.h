#pragma once
#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <NTSecAPI.h>
#include <sddl.h> 
#include <locale.h>
#pragma comment( lib, "shlwapi.lib")

VOID ShowHelp();
VOID ShowAwesomeBanner();
DWORD InitMode1(wchar_t* cAccName, wchar_t* cPrivName);
DWORD InitMode2(wchar_t* cPath, wchar_t* cPrivName);
DWORD InitMode3(wchar_t* cAccName, wchar_t* cPrivName);
DWORD InitMode4(wchar_t* cCompName, wchar_t* cPrivName);
DWORD InitMode5(wchar_t* cCompName, wchar_t* cUsername);

DWORD AddUserPrivilege(LSA_HANDLE hPolicy, LPWSTR wUsername, LPWSTR wPrivName, BOOL bEnable);
DWORD GetPolicy(PLSA_HANDLE LsaHandle);
DWORD PrintTrusteePrivs(LSA_HANDLE hPolicy, PSID psid);
DWORD EnableTokenPrivilege(HANDLE hToken, LPTSTR szPriv, BOOL bEnabled);
DWORD ValidateAccInfo(wchar_t* cAccName, wchar_t* cPrivName);
DWORD ValidatePathInfo(wchar_t* Path, wchar_t* cPrivName);