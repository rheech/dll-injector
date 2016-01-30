#include "stdafx.h"

BOOL IsVistaOrHigher()
{
    OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    // check if kernel version is higher than or equal to 6
    if( osvi.dwMajorVersion >= 6 )
        return TRUE;

    return FALSE;
}

BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
{
    HANDLE hThread = NULL;
    FARPROC pFunc = NULL;

    if(IsVistaOrHigher())    // for Vista, 7, Server 2008
    {
        pFunc = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtCreateThreadEx");
        if( pFunc == NULL )
        {
            return FALSE;
        }

        // call NtCreateThreadEx()
        ((PFNTCREATETHREADEX)pFunc)(&hThread,
                                    0x1FFFFF,
                                    NULL,
                                    hProcess,
                                    pThreadProc,
                                    pRemoteBuf,
                                    FALSE,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL);
        if( hThread == NULL )
        {
            return FALSE;
        }
    }
    else // for 2000, XP, Server 2003
    {
        hThread = CreateRemoteThread(hProcess, 
                                     NULL, 
                                     0, 
                                     pThreadProc, 
                                     pRemoteBuf, 
                                     0, 
                                     NULL);
        if( hThread == NULL )
        {
            return FALSE;
        }
    }

    if( WAIT_FAILED == WaitForSingleObject(hThread, INFINITE) )
    {
        return FALSE;
    }

    return TRUE;
}

LPSTR WtoA(LPCWSTR string)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, string, -1, NULL, 0, NULL, NULL);

	CHAR* rtnStr;
	rtnStr = new CHAR[nLen + 1];
	WideCharToMultiByte(CP_ACP, 0, string, -1, rtnStr, nLen, NULL, NULL);
	rtnStr[nLen] = 0;

	return rtnStr;
}

BOOL InjectDll(DWORD dwPID, LPCWSTR szDllName)
{
	BOOL rtn;
	LPSTR dllName;
	dllName = WtoA(szDllName);

	rtn = InjectDll(dwPID, dllName);
	
	delete [] dllName;

	return rtn;
}

BOOL InjectDll(DWORD dwPID, LPCSTR szDllName)
{
    HANDLE hProcess = NULL;
    LPVOID pRemoteBuf = NULL;
    FARPROC pThreadProc = NULL;
    DWORD dwBufSize = strlen(szDllName)+1;

    if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        return FALSE;
    }

    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, 
                                MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, 
                       dwBufSize, NULL);

    pThreadProc = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), 
                                 "LoadLibraryA");

    if( !MyCreateRemoteThread(hProcess, (LPTHREAD_START_ROUTINE)pThreadProc, pRemoteBuf) )
    {
        return FALSE;
    }

    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);

    CloseHandle(hProcess);

    return TRUE;
}


BOOL IsUserAdmin(VOID)
/*++ 
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token. 
Arguments: None. 
Return Value: 
TRUE - Caller has Administrators local group. 
FALSE - Caller does not have Administrators local group. --
*/ 
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup); 
	if(b) 
	{
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
		{
			b = FALSE;
		} 
		FreeSid(AdministratorsGroup); 
	}

	return(b);
}

BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName)
{
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = { sizeof(me) };
    LPTHREAD_START_ROUTINE pThreadProc;

	TCHAR dllName[MAX_PATH];
	
	if (szDllName != NULL)
		_tcscpy_s(dllName, MAX_PATH, szDllName);

	PathStripPath(dllName);

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    bMore = Module32First(hSnapshot, &me);

    while((bMore = Module32Next(hSnapshot, &me)))
    {
        if( !_tcsicmp((LPCTSTR)me.szModule, dllName) )
        {
            bFound = TRUE;
            break;
        }
    }

    if( !bFound )
    {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    hModule = GetModuleHandle(_T("kernel32.dll"));
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
    hThread = CreateRemoteThread(hProcess, NULL, 0,
                                 pThreadProc, me.modBaseAddr,
                                 0, NULL);
    WaitForSingleObject(hThread, INFINITE); 

    CloseHandle(hThread);
    CloseHandle(hProcess);
    CloseHandle(hSnapshot);

    return TRUE;
}