#pragma once

#define LBN_SUBMIT (WM_USER + 1)

// MyCEdit

class MyCEdit : public CEdit
{
	DECLARE_DYNAMIC(MyCEdit)

public:
	MyCEdit();
	void SetSubmitButton(CButton* pBtn);
	virtual ~MyCEdit();

protected:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	CButton* pButton;
	DECLARE_MESSAGE_MAP()
};


