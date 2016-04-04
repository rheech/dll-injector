// dllInjectDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "mycedit.h"
#include "afxwin.h"

// CdllInjectDlg dialog
class CdllInjectDlg : public CDialog
{
// Construction
public:
	CdllInjectDlg(CWnd* pParent = NULL);	// standard constructor
	CdllInjectDoc* GetDocument() const;
	~CdllInjectDlg();

// Dialog Data
	enum { IDD = IDD_DLLINJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	CdllInjectDoc* _doc;

	virtual BOOL PreTranslateMessage(MSG *pMsg);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();

	// List Control for listing process
	CListCtrl lvProcess;
	// test TextBox
	MyCEdit ctxtDllPath;
	afx_msg void OnBnClickedCheck1();
	// CString points DLL Path text
	CString txtDllPath;
	CStringA txtDllPathA;
	// Process Name for Automatic Inject
	MyCEdit ctxtProcessName;
	afx_msg void OnBnClickedButton5();
	// check value to automatic inject
	BOOL chkAuto;
	// button start inject
	CButton btnStart;
	CButton btnInject;
	CButton btnEject;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CString txtFileName;
	CButton btnBrowse;
	CButton cchkAuto;
	CString txtProcessName;
	// automatic injection started
	bool autoInjStarted;
	CEdit ctxtLog;
	CString txtLog;
	void writeLog(CString &szLine);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	void OnOK(void);
};
