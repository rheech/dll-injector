// mfcDllInjectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcDllInjector.h"
#include "mfcDllInjectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CmfcDllInjectorDlg dialog




CmfcDllInjectorDlg::CmfcDllInjectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmfcDllInjectorDlg::IDD, pParent)
	, txtPid(_T(""))
	, txtImgName(_T(""))
	, txtDllPath(_T(""))
	, chkAutoUpdate(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfcDllInjectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, plist);
	DDX_Text(pDX, IDC_EDIT1, txtPid);
	DDX_Text(pDX, IDC_EDIT2, txtImgName);
	DDX_Text(pDX, IDC_EDIT3, txtDllPath);
	DDX_Check(pDX, IDC_CHECK1, chkAutoUpdate);
	DDX_Control(pDX, IDC_EDIT2, edtImgName);
}

BEGIN_MESSAGE_MAP(CmfcDllInjectorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CmfcDllInjectorDlg::OnLvnItemchangedList1)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST1, &CmfcDllInjectorDlg::OnLvnItemActivateList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CmfcDllInjectorDlg::OnNMClickList1)
	ON_EN_CHANGE(IDC_EDIT2, &CmfcDllInjectorDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON1, &CmfcDllInjectorDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_BUTTON2, &CmfcDllInjectorDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CmfcDllInjectorDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON4, &CmfcDllInjectorDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CmfcDllInjectorDlg::OnBnClickedInject)
	ON_BN_CLICKED(IDC_BUTTON5, &CmfcDllInjectorDlg::OnBnClickedEject)
END_MESSAGE_MAP()


// CmfcDllInjectorDlg message handlers

BOOL CmfcDllInjectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// create columns in listview
	plist.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	plist.InsertColumn(0, _T("ImageName"), LVCFMT_LEFT, 200, -1);
	plist.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 100, -1);

	if (IsUserAdmin() == TRUE)
	{
		CString wndText;
		GetWindowText(wndText);

		wndText += _T(" [Administrator]");

		SetWindowText(wndText.GetBuffer(0));
	}

	updateProcList();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CmfcDllInjectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmfcDllInjectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmfcDllInjectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CmfcDllInjectorDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CmfcDllInjectorDlg::OnBnClickedOk()
{
	updateProcList();

	// TODO: Add your control notification handler code here
	//OnOK();
}

void CmfcDllInjectorDlg::OnLvnItemActivateList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CmfcDllInjectorDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	txtPid = _T("");
	
	POSITION ps;
	ps = plist.GetFirstSelectedItemPosition();
	
	int k;

	while((k = plist.GetNextSelectedItem(ps)) != -1)
	{
		CString tmpTxt;
		int len;
		tmpTxt = plist.GetItemText(k, 1);

		
		txtPid += tmpTxt + _T(";");
	}

	UpdateData(FALSE);
}

void CmfcDllInjectorDlg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here


}

void CmfcDllInjectorDlg::OnBnClickedSearch()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	
	int k;

	CString tmpTxt;

	for (int i = 0; i < plist.GetItemCount(); i++)
	{
		tmpTxt = plist.GetItemText(i, 0);

		if (!_tcsicmp(txtImgName.GetBuffer(0), tmpTxt.GetBuffer(0)))
			plist.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}

	NMHDR nm;
	nm.code = 0;
	nm.hwndFrom = this->GetSafeHwnd();
	nm.idFrom = NULL;

	LONG rst;

	OnNMClickList1(&nm, &rst);
}

void CmfcDllInjectorDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, _T("*.*"), _T(""),
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
		_T("Microsoft Dynamic Link Library (*.dll)|*.dll||"), this);

	if (dlg.DoModal() == IDOK)
	{
		txtDllPath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CmfcDllInjectorDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);

	if (chkAutoUpdate == TRUE)
		setTimer(TRUE);
	else
		setTimer(FALSE);
}

void CmfcDllInjectorDlg::setTimer(BOOL enabled)
{
	if (enabled == TRUE)
		SetTimer(0, 1000, NULL);
	else
		KillTimer(0);
}

void CmfcDllInjectorDlg::updateProcList()
{
	this->plist.DeleteAllItems();

	HANDLE hSnap;

	PROCESSENTRY32 t;
	t.dwSize = sizeof(t);

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hSnap, &t);

	int item = plist.InsertItem(0, t.szExeFile, 0);

	// print pid to next column
	TCHAR pid[20];
	TCHAR imgPath[MAX_PATH];

	wsprintf(pid, _T("%d"), t.th32ProcessID);
	plist.SetItem(item, 1, LVIF_TEXT, pid, 0, 0,  0, NULL);

	while(Process32Next(hSnap, &t))
	{
		item = plist.InsertItem(1, t.szExeFile, 0);
		wsprintf(pid, _T("%d"), t.th32ProcessID);
		plist.SetItem(item, 1, LVIF_TEXT, pid, 0, 0,  0, NULL);
	}
}

void CmfcDllInjectorDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 0)
	{
		this->updateProcList();
	}
}
void CmfcDllInjectorDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	this->updateProcList();
}

void CmfcDllInjectorDlg::OnBnClickedInject()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (txtDllPath == _T("") || txtPid == _T(""))
		return;

	CString tmp;
	DWORD dwPid;

	LPTSTR pToken;

	CString txtPidCpy;
	txtPidCpy = txtPid;

	pToken = _tcstok(txtPidCpy.GetBuffer(0), _T(";"));

	CString errOut;

	int errCnt = 0;
	TCHAR lpEmpty[10] = {0};
	TCHAR lpTmpPid[10] = {0};

	if (pToken != NULL)
	{
		dwPid = _ttoi(pToken);
		
		if (dwPid != 0)
			if (!InjectDll(dwPid, txtDllPath.GetBuffer(0)))
			{
				_itot(dwPid, lpTmpPid, 10);
				errOut += lpTmpPid;
				errCnt++;
			}
	}

	
	while ((pToken = _tcstok(NULL, _T(";"))) != NULL)
	{
		dwPid = _ttoi(pToken);
		
		if (dwPid != 0)
			if (!InjectDll(dwPid, txtDllPath.GetBuffer(0)))
			{
				errOut += _T(", ");

				if (errCnt % 5 == 0)
					errOut += _T("\n");

				_tcscpy_s(lpTmpPid, 10, lpEmpty);
				_itot(dwPid, lpTmpPid, 10);
				errOut += lpTmpPid;
			}
	}

	CString msg;

	if (errCnt != 0)
	{
		msg = _T("The following PIDs are not injected:\n")
			 + errOut;
		MessageBox(msg.GetBuffer(0), _T("Message"), MB_ICONEXCLAMATION);
	}
	else
	{
		msg = _T("Operation succeeded.");
		MessageBox(msg.GetBuffer(0), _T("Message"), MB_ICONINFORMATION);
	}
}

void CmfcDllInjectorDlg::OnBnClickedEject()
{
	if (txtDllPath == _T("") || txtPid == _T(""))
		return;

	CString tmp;
	DWORD dwPid;

	LPTSTR pToken;

	CString txtPidCpy;
	txtPidCpy = txtPid;

	pToken = _tcstok(txtPidCpy.GetBuffer(0), _T(";"));

	CString errOut;

	int errCnt = 0;
	TCHAR lpEmpty[10] = {0};
	TCHAR lpTmpPid[10] = {0};

	if (pToken != NULL)
	{
		dwPid = _ttoi(pToken);
		
		if (dwPid != 0)
			if (!EjectDll(dwPid, txtDllPath.GetBuffer(0)))
			{
				_itot(dwPid, lpTmpPid, 10);
				errOut += lpTmpPid;
				errCnt++;
			}
	}

	
	while ((pToken = _tcstok(NULL, _T(";"))) != NULL)
	{
		dwPid = _ttoi(pToken);
		
		if (dwPid != 0)
			if (!EjectDll(dwPid, txtDllPath.GetBuffer(0)))
			{
				errOut += _T(", ");

				if (errCnt % 5 == 0)
					errOut += _T("\n");

				_tcscpy_s(lpTmpPid, 10, lpEmpty);
				_itot(dwPid, lpTmpPid, 10);
				errOut += lpTmpPid;
			}
	}

	CString msg;

	if (errCnt != 0)
	{
		msg = _T("The following PIDs are not ejected:\n")
			 + errOut;
		MessageBox(msg.GetBuffer(0), _T("Message"), MB_ICONEXCLAMATION);
	}
	else
	{
		msg = _T("Operation succeeded.");
		MessageBox(msg.GetBuffer(0), _T("Message"), MB_ICONINFORMATION);
	}
}

BOOL CmfcDllInjectorDlg::PreTranslateMessage(MSG* pMsg)
{
    // Enter¿Í Esc ¸·±â

    if(pMsg->message == WM_KEYDOWN)
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	if(pMsg->message == 9999)
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;

	//CEdit* txtImgName = (CEdit*)GetDlgItem(IDC_EDIT2);

    return CDialog::PreTranslateMessage(pMsg);

}
