#include "StdAfx.h"
#include "CdllInjectDoc.h"
#include <Tlhelp32.h>

// contains internal operations

CdllInjectDoc::CdllInjectDoc(void)
{
	ctxtLog = NULL;
}

CdllInjectDoc::~CdllInjectDoc(void)
{
}

void CdllInjectDoc::listAllProcess(CListCtrl* lv)
{
	// create Process Snapshot
	HANDLE hSnapshot;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};

	// first snapshot
	Process32First(hSnapshot, &pe);
	
	// index, pid string
	int index = 0;
	
	TCHAR pid[20];
	TCHAR emptyPid[20] = {0};

	wsprintf(pid, _T("%d"), pe.th32ProcessID);

	// insert first item
	index = lv->InsertItem(0, pe.szExeFile);
	lv->SetItem(index, 1, LVIF_TEXT, pid, 0, 0, 0, NULL);

	// empty pid string
	memcpy_s(pid, 20, emptyPid, 20);

	// insert next items
	int i = index + 1;

	while(Process32Next(hSnapshot, &pe) != FALSE)
	{
		index = lv->InsertItem(i, pe.szExeFile);
		wsprintf(pid, _T("%d"), pe.th32ProcessID);
		lv->SetItem(index, 1, LVIF_TEXT, pid, 0, 0, 0, NULL);

		i++;
	}

	CloseHandle(hSnapshot);
}

void CdllInjectDoc::updateProcList(CListCtrl* lv, LPCTSTR imgName, LPSTR dllName)
{
	// create Process Snapshot
	HANDLE hSnapshot;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};

	// first snapshot
	Process32First(hSnapshot, &pe);
	
	// index, pid string
	int index = 0;
	
	TCHAR pid[20];
	TCHAR emptyPid[20] = {0};

	wsprintf(pid, _T("%d"), pe.th32ProcessID);

	// 1st, 2nd item text (exe file, pid)
	CString first, second;
	first = lv->GetItemText(0, 0);
	second = lv->GetItemText(0, 1);

	// update first process information
	if (pe.th32ProcessID != _ttoi(second.GetBuffer(0)) ||
			_tcsicmp(pe.szExeFile, first.GetBuffer(0)) != 0)
	{
		lv->DeleteItem(0);
		index = lv->InsertItem(0, pe.szExeFile);
		lv->SetItem(index, 1, LVIF_TEXT, pid, 0, 0, 0, NULL);
	}

	// check automatic inject option
	if (imgName != NULL &&
			_tcsicmp(imgName,pe.szExeFile) == 0)
	{
		InjectDll(pe.th32ProcessID, dllName);
	}

	// empty pid string
	//memcpy_s(pid, 20, emptyPid, 20);

	// insert next items
	int i = index + 1;

	// update next process informations
	while(Process32Next(hSnapshot, &pe) != FALSE)
	{
		first = lv->GetItemText(i, 0);
		second = lv->GetItemText(i, 1);

		// check automatic inject option
		if (imgName != NULL &&
			_tcsicmp(imgName, pe.szExeFile) == 0)
		{
			// empty pid string
			memcpy_s(pid, 20, emptyPid, 20);

			_itot_s(pe.th32ProcessID, pid, 20, 10);

			CString temp;
			temp.Append(pe.szExeFile);
			temp += _T("|");
			temp.Append(pid);
			temp += _T("|");

			if (pidInfo.Find(temp, 0) != -1)
			{
				i++;
				continue;
			}

			TCHAR line[260];
			_stprintf_s(line, 260, _T("Injecting %s(%d)... "),
						pe.szExeFile, pe.th32ProcessID);

			writeLog(CString(line));

			if (InjectDll(pe.th32ProcessID, dllName) == TRUE)
			{
				writeLog(CString("SUCCEED.\r\n"));
				pidInfo += temp;
			}
			else
				writeLog(CString("FAILED.\r\n"));
		}

		// if already in listview, skip
		if (pe.th32ProcessID == _ttoi(second.GetBuffer(0)) &&
				_tcsicmp(pe.szExeFile, first.GetBuffer(0)) == 0)
		{
			i++;
			continue;
		}

		// delete old process info, and inform that is exited & delete from injection list
		CString temp;
		temp = first + _T("|") + second + _T("|");

		if (pidInfo.Find(temp) != -1)
		{
			CString output;
			output = _T("Process Exited: ") + first 
				+ _T("(") + second + _T(").")
				+ _T("\r\n");

			writeLog(output);

			pidInfo.Replace(temp, _T(""));
		}


		lv->DeleteItem(i);
		index = lv->InsertItem(i, pe.szExeFile);
		wsprintf(pid, _T("%d"), pe.th32ProcessID);
		lv->SetItem(index, 1, LVIF_TEXT, pid, 0, 0, 0, NULL);

		i++;
	}

	CloseHandle(hSnapshot);

	// if listcount is more than process list, delete records
	while (i < lv->GetItemCount())
	{
		// delete old process info, and inform that is exited & delete from injection list
		first = lv->GetItemText(i, 0);
		second = lv->GetItemText(i, 1);

		CString temp;
		temp = first + _T("|") + second + _T("|");

		if (pidInfo.Find(temp) != -1)
		{
			CString output;
			output = _T("Process Exited: ") + first 
					+ _T("(") + second + _T(").")
					+ _T("\r\n");

			writeLog(output);

			pidInfo.Replace(temp, _T(""));
		}

		lv->DeleteItem(i);

		i++;
	}
}

// command for inject button
void CdllInjectDoc::InjectSelected(CListCtrl* lv, LPSTR szDllName)
{
	POSITION pos = lv->GetFirstSelectedItemPosition();
	
	int index = 0;
	TCHAR pid[21];
	TCHAR cPid[21] = {0};
	DWORD dwPid;

	while((index = lv->GetNextSelectedItem(pos)) != -1)
	{
		memcpy_s(pid, 21, cPid, 21);

		lv->GetItemText(index, 1, pid, 20);
		dwPid = _ttoi(pid);

		TCHAR exeFile[MAX_PATH];
		lv->GetItemText(index, 0, exeFile, MAX_PATH);

		TCHAR line[260];
		_stprintf_s(line, 260, _T("Injecting %s(%s)... "), exeFile, pid);		

		writeLog(CString(line));

		CString temp;
		temp.Append(exeFile);
		temp += _T("|");
		temp.Append(pid);
		temp += _T("|");

		int aa = pidInfo.Find(temp, 0);

		if (pidInfo.Find(temp, 0) != -1)
		{
			writeLog(CString("Already Injected.\r\n"));
			continue;
		}

		if (InjectDll(dwPid, szDllName) == TRUE)
		{
			writeLog(CString("SUCCEED.\r\n"));
			pidInfo += temp;
		}
		else
			writeLog(CString("FAILED.\r\n"));
	}
}

void CdllInjectDoc::EjectSelected(CListCtrl* lv, LPTSTR szDllName)
{
	POSITION pos = lv->GetFirstSelectedItemPosition();
	
	int index = 0;
	TCHAR pid[21];
	TCHAR cPid[21] = {0};
	DWORD dwPid;

	while((index = lv->GetNextSelectedItem(pos)) != -1)
	{
		memcpy_s(pid, 21, cPid, 21);

		lv->GetItemText(index, 1, pid, 20);
		dwPid = _ttoi(pid);

		TCHAR exeFile[MAX_PATH];
		lv->GetItemText(index, 0, exeFile, MAX_PATH);

		TCHAR line[260];
		_stprintf_s(line, 260, _T("Ejecting %s(%s)... "), exeFile, pid);

		writeLog(CString(line));

		if (EjectDll(dwPid, szDllName) == TRUE)
		{
			CString temp;
			temp.Append(exeFile);
			temp += _T("|");
			temp.Append(pid);
			temp += _T("|");

			pidInfo.Replace(temp, _T(""));

			writeLog(CString("SUCCEED.\r\n"));
		}
		else
			writeLog(CString("FAILED.\r\n"));
	}
}

void CdllInjectDoc::setLogFunc(CEdit* LogFunc)
{
	ctxtLog = LogFunc;
}

void CdllInjectDoc::writeLog(CString &szLine)
{
	if (ctxtLog == NULL)
		return;

	CString logText;

	ctxtLog->GetWindowTextW(logText);

	if (logText == _T(""))
		logText += szLine;
	else
		logText += _T("") + szLine;

	ctxtLog->SetWindowText(logText.GetBuffer(0));


	int nLength = ctxtLog->GetWindowTextLength();
	 // put the selection at the end of text
	ctxtLog->SetSel(nLength, nLength);
	 // replace the selection
	//ctxtLog->ReplaceSel(logText.GetBuffer(0));


}
BOOL CdllInjectDoc::isUserAdmin(void)
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
