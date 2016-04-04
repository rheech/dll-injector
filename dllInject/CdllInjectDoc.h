#pragma once

typedef void (*WRITELOG)(CString &szLine);

class CdllInjectDoc
{
public:
	CdllInjectDoc(void);
	~CdllInjectDoc(void);

	void listAllProcess(CListCtrl* lv);
	void updateProcList(CListCtrl* lv, LPCTSTR imgName, LPSTR dllName);
	// command for inject button
	void InjectSelected(CListCtrl* lv, LPSTR szDllName);
	void EjectSelected(CListCtrl* lv, LPTSTR szDllName);
	void setLogFunc(CEdit* LogFunc);
	void writeLog(CString &szLine);

protected:
	CEdit* ctxtLog;
	CString pidInfo;
public:
	BOOL isUserAdmin(void);
};
