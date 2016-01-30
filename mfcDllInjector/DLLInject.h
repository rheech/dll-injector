BOOL IsVistaOrHigher();
BOOL MyCreateRemoteThread (HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf);
BOOL InjectDll(DWORD dwPID, LPCSTR szDllName);
BOOL InjectDll(DWORD dwPID, LPCWSTR szDllName);
LPSTR WtoA(LPCWSTR string);
BOOL IsUserAdmin(VOID) /*++ Routine Description: This routine returns TRUE if the caller's process is a member of the Administrators local group. Caller is NOT expected to be impersonating anyone and is expected to be able to open its own process and process token. Arguments: None. Return Value: TRUE - Caller has Administrators local group. FALSE - Caller does not have Administrators local group. -- */;
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName);

typedef DWORD (WINAPI *PFNTCREATETHREADEX)
( 
    PHANDLE                 ThreadHandle,
    ACCESS_MASK             DesiredAccess,
    LPVOID                  ObjectAttributes,
    HANDLE                  ProcessHandle,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    BOOL                    CreateSuspended,
    DWORD                   dwStackSize,
    DWORD                   dw1, 
    DWORD                   dw2, 
    LPVOID                  Unknown 
	); 
