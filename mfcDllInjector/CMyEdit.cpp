#include "stdafx.h"


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_MESSAGE(WM_KEYDOWN, OnKeyDown)
	ON_MESSAGE(WM_KEYUP, OnKeyUp)
END_MESSAGE_MAP()

LRESULT CMyEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	//if ('\t' == w) {
	//	return 0;
	//}
	
	return DefWindowProc(WM_KEYDOWN, wParam, lParam);
}

LRESULT CMyEdit::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	//if ('\t' == w ) {
	//	DWORD s = GetSel();
	//	if (0 != s && LOWORD(s) == HIWORD(s)) {
	//		//ReplaceSel(_T("\t"));
	//                       return 0;
	//	}
	//}
	
	if (wParam == VK_RETURN)
	{		
		return DefWindowProc(WM_KEYUP, wParam, lParam);
	}
//ON_BN_CLICKED(IDC_BUTTON1, &CmfcDllInjectorDlg::OnBnClickedSearch)

	return DefWindowProc(WM_KEYUP, wParam, lParam);
}
