// mfcDllInjectorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CmfcDllInjectorDlg dialog
class CmfcDllInjectorDlg : public CDialog
{
// Construction
public:
	CmfcDllInjectorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCDLLINJECTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl plist;
	afx_msg void OnBnClickedOk();
	// Process id Textbox
	afx_msg void OnLvnItemActivateList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	// Textbox contains pid
	CString txtPid;
	CString txt;
	// Textbox to Image Name
	CString txtImgName;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedButton2();
	// Textbox to DLL Path
	CString txtDllPath;
	// Checkbox to Auto update
	BOOL chkAutoUpdate;
	afx_msg void OnBnClickedCheck1();

	void setTimer(BOOL enabled);
	VOID CALLBACK onTimer(HWND hwnd,
		UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	void OnTimer(UINT nIDEvent);
	void updateProcList();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedEject();
	
	BOOL PreTranslateMessage(MSG* pMsg);
	CEdit edtImgName;
	void OnKeyUp(UINT Message, UINT wParam, UINT lParam);
};
