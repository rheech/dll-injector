// MyCEdit.cpp : implementation file
//

#include "stdafx.h"
#include "dllInject.h"
#include "MyCEdit.h"


// MyCEdit

IMPLEMENT_DYNAMIC(MyCEdit, CEdit)

MyCEdit::MyCEdit()
{
	pButton = NULL;
}

MyCEdit::~MyCEdit()
{
}


BEGIN_MESSAGE_MAP(MyCEdit, CEdit)
END_MESSAGE_MAP()

BOOL MyCEdit::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN &&
				pButton != NULL)
		{
			//pButton->SetFocus();
			GetParent()->SendMessage(WM_COMMAND,
				(WPARAM)MAKELONG(::GetDlgCtrlID(pButton->GetSafeHwnd()), BN_CLICKED),
				(LPARAM)GetSafeHwnd());
			pButton->SetFocus();
		}
		
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void MyCEdit::SetSubmitButton(CButton* pBtn)
{
	pButton = pBtn;
}

// MyCEdit message handlers


