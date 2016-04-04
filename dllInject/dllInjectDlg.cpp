// dllInjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dllInject.h"
#include "dllInjectDlg.h"

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


// CdllInjectDlg dialog

CdllInjectDlg::CdllInjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CdllInjectDlg::IDD, pParent)
	, txtDllPath(_T(""))
	, chkAuto(FALSE)
	, txtFileName(_T(""))
	, txtProcessName(_T(""))
	, autoInjStarted(false)
	, txtLog(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// get Document component

CdllInjectDoc* CdllInjectDlg::GetDocument() const
{
	return _doc;
}

CdllInjectDlg::~CdllInjectDlg()
{
	delete _doc;
}

void CdllInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, lvProcess);
	DDX_Control(pDX, IDC_EDIT1, ctxtDllPath);
	DDX_Text(pDX, IDC_EDIT1, txtDllPath);
	DDX_Control(pDX, IDC_EDIT2, ctxtProcessName);
	DDX_Check(pDX, IDC_CHECK1, chkAuto);
	DDX_Control(pDX, IDC_BUTTON5, btnStart);
	DDX_Control(pDX, IDC_BUTTON2, btnInject);
	DDX_Control(pDX, IDC_BUTTON3, btnEject);
	DDX_Control(pDX, IDC_BUTTON1, btnBrowse);
	DDX_Control(pDX, IDC_CHECK1, cchkAuto);
	DDX_Text(pDX, IDC_EDIT2, txtProcessName);
	DDX_Control(pDX, IDC_EDIT4, ctxtLog);
	DDX_Text(pDX, IDC_EDIT4, txtLog);
}

BEGIN_MESSAGE_MAP(CdllInjectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT1, &CdllInjectDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CdllInjectDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &CdllInjectDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON5, &CdllInjectDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON2, &CdllInjectDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CdllInjectDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CdllInjectDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CdllInjectDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CdllInjectDlg message handlers

BOOL CdllInjectDlg::OnInitDialog()
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

	// Initialize Document
	_doc = new CdllInjectDoc();

	// Insert columns in listview
	lvProcess.InsertColumn(1, _T("ProcessName"), 0, 150);
	lvProcess.InsertColumn(1, _T("PID"), 0, 50);

	// set listview style to full row select
	lvProcess.SetExtendedStyle(lvProcess.GetExtendedStyle() |
					LVS_EX_FULLROWSELECT);

	_doc->listAllProcess(&lvProcess);

	// Enable Timer that allows to update process list
	SetTimer(0, 1000, NULL);

	// set submit button
	//ctxtDllPath.SetSubmitButton((CButton*)GetDlgItem(IDC_BUTTON1));
	ctxtProcessName.SetSubmitButton((CButton*)GetDlgItem(IDC_BUTTON5));

	// set log writer
	_doc->setLogFunc(&ctxtLog);

	// if useradmin, notice to window text
	if (_doc->isUserAdmin())
	{
		CString wndText;
		GetWindowText(wndText);

		wndText += _T(" [Administrator]");

		SetWindowText(wndText.GetBuffer(0));
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CdllInjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CdllInjectDlg::OnPaint()
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
HCURSOR CdllInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// timer callback
void CdllInjectDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 0:
		if (autoInjStarted)
			_doc->updateProcList(&lvProcess,
			txtProcessName.GetBuffer(0), 
			txtDllPathA.GetBuffer(0));
		else
			_doc->updateProcList(&lvProcess, NULL, NULL);
		break;
	}
}

void CdllInjectDlg::OnDestroy()
{
	KillTimer(0);
}

void CdllInjectDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CdllInjectDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, _T("Microsoft Windows Dynamic-Link Library"), _T(""),
					OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
					_T("Microsoft Windows Dynamic-Link Library (*.dll)|*.dll"),
					this);

	if (dlg.DoModal() == IDOK)
	{
		txtDllPath = dlg.GetPathName();
		txtDllPathA = CString(txtDllPath);
		txtFileName = dlg.GetFileName();
		UpdateData(FALSE);
	}
}

BOOL CdllInjectDlg::PreTranslateMessage( MSG *pMsg )
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// disable IDOK, IDCANCEL
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
void CdllInjectDlg::OnBnClickedCheck1()
{
	UpdateData(TRUE);

	// TODO: Add your control notification handler code here
	if (chkAuto == TRUE)
	{
		ctxtProcessName.EnableWindow(TRUE);
		btnStart.EnableWindow(TRUE);

		btnInject.EnableWindow(FALSE);
		btnEject.EnableWindow(FALSE);
		//lvProcess.EnableWindow(FALSE);
	}
	else
	{
		ctxtProcessName.EnableWindow(FALSE);
		btnStart.EnableWindow(FALSE);

		btnInject.EnableWindow(TRUE);
		btnEject.EnableWindow(TRUE);
		//lvProcess.EnableWindow(TRUE);
	}
}

void CdllInjectDlg::OnBnClickedButton5()
{
	static BOOL isStarted = FALSE;

	UpdateData(TRUE);

	if (txtProcessName == _T(""))
	{
		MessageBox(_T("Please put the process name."), _T("Error"), MB_ICONERROR);
		return;
	}
	
	if (txtDllPath == _T(""))
	{
		MessageBox(_T("Please put the dll path."), _T("Error"), MB_ICONERROR);
		return;
	}

	
	if (isStarted == FALSE)
	{
		
		btnBrowse.EnableWindow(FALSE);
		cchkAuto.EnableWindow(FALSE);
		ctxtProcessName.EnableWindow(FALSE);
		btnStart.SetWindowTextW(_T("Stop"));
		isStarted = TRUE;
		autoInjStarted = true;
	}
	else
	{
		btnBrowse.EnableWindow(TRUE);
		cchkAuto.EnableWindow(TRUE);
		ctxtProcessName.EnableWindow(TRUE);
		btnStart.SetWindowTextW(_T("Start"));
		isStarted = FALSE;
		autoInjStarted = false;
	}

	// TODO: Add your control notification handler code here
}

void CdllInjectDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	if (lvProcess.GetSelectedCount() == 0)
	{
		MessageBox(_T("Please select the process."), _T("Error"), MB_ICONERROR);
		return;
	}
	
	if (txtDllPath == _T(""))
	{
		MessageBox(_T("Please put the dll path."), _T("Error"), MB_ICONERROR);
		return;
	}

	// TODO: Add your control notification handler code here
	_doc->InjectSelected(&lvProcess, txtDllPathA.GetBuffer(0));
}

void CdllInjectDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);

	if (lvProcess.GetSelectedCount() == 0)
	{
		MessageBox(_T("Please select the process."), _T("Error"), MB_ICONERROR);
		return;
	}
	
	if (txtDllPath == _T(""))
	{
		MessageBox(_T("Please put the dll path."), _T("Error"), MB_ICONERROR);
		return;
	}

	// TODO: Add your control notification handler code here

	_doc->EjectSelected(&lvProcess, txtFileName.GetBuffer(0));
}

void CdllInjectDlg::writeLog(CString &szLine)
{
	UpdateData(TRUE);

	if (txtLog == _T(""))
		txtLog += szLine;
	else
		txtLog += _T("\n") + szLine;

	UpdateData(FALSE);
}

void CdllInjectDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	txtLog = _T("");
	UpdateData(FALSE);
}

void CdllInjectDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CdllInjectDlg::OnOK(void)
{
	return;
}
