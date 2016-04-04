#include "stdafx.h"
#include <tlhelp32.h>

typedef DWORD (WINAPI *PFNTCREATETHREADEX)
( 
    PHANDLE                 ThreadHandle,	
    ACCESS_MASK             DesiredAccess,	
    LPVOID                  ObjectAttributes,	
    HANDLE                  ProcessHandle,	
    LPTHREAD_START_ROUTINE  lpStartAddress,	
    LPVOID                  lpParameter,	
    BOOL	                   CreateSuspended,	
    DWORD                   dwStackSize,	
    DWORD                   dw1, 
    DWORD                   dw2, 
    LPVOID                  Unknown 
); 


BOOL IsVistaOrLater()
{
    OSVERSIONINFO osvi;


    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);


    GetVersionEx(&osvi);


    // 커널 버전이 6 이상인지 확인!
    if( osvi.dwMajorVersion >= 6 )
        return TRUE;


    return FALSE;
}


BOOL MyCreateRemoteThread
(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
{
    HANDLE      hThread = NULL;
    FARPROC     pFunc = NULL;


    // OS 가 Vista 이상인지 확인!
    if( IsVistaOrLater() )    // Vista, 7, Server2008
    {
        pFunc = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtCreateThreadEx");
        if( pFunc == NULL )
        {
            //printf("GetProcAddress(\"NtCreateThreadEx\") failed!!! [%d]\n",
//                   GetLastError());
            return FALSE;
        }


        // NtCreateThreadEx() 호출
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
            //printf("NtCreateThreadEx() failed!!! [%d]\n", GetLastError());
            return FALSE;
        }
    }
    else                    // 2000, XP, Server2003
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
            //printf("CreateRemoteThread() failed!!! [%d]\n", GetLastError());
            return FALSE;
        }
    }


    if( WAIT_FAILED == WaitForSingleObject(hThread, INFINITE) )
    {
        //printf("WaitForSingleObject() failed!!! [%d]\n", GetLastError());
        return FALSE;
    }


    return TRUE;
}


BOOL InjectDll(DWORD dwPID, char *szDllName)
{
    HANDLE hProcess = NULL;
    LPVOID pRemoteBuf = NULL;
    FARPROC pThreadProc = NULL;
    DWORD dwBufSize = strlen(szDllName)+1;


    if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        //printf("OpenProcess(%d) failed!!! [%d]\n", 
        //        dwPID, GetLastError());
        return FALSE;
    }


    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, 
                                MEM_COMMIT, PAGE_READWRITE);


    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllName, 
                       dwBufSize, NULL);


    pThreadProc = GetProcAddress(GetModuleHandle(L"kernel32.dll"), 
                                 "LoadLibraryA");


    if( !MyCreateRemoteThread(hProcess, (LPTHREAD_START_ROUTINE)pThreadProc, pRemoteBuf) )
    {
        //printf("CreateRemoteThread() failed!!! [%d]\n", GetLastError());
        return FALSE;
    }


    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);


    CloseHandle(hProcess);


    return TRUE;
}

BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName)
{
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = { sizeof(me) };
    LPTHREAD_START_ROUTINE pThreadProc;

    // dwPID = notepad 프로세스 ID
    // TH32CS_SNAPMODULE 파라미터를 이용해서 notepad 프로세스에 로딩된 DLL 이름을 얻음
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    bMore = Module32First(hSnapshot, &me);
    for( ; bMore ; bMore = Module32Next(hSnapshot, &me) )
    {
        if( !_tcsicmp((LPCTSTR)me.szModule, szDllName) )
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
