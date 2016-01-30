typedef void (*FNCVOID)();

class CMyEdit : public CEdit
{
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnKeyDown(WPARAM, LPARAM);
	afx_msg LRESULT OnKeyUp(WPARAM, LPARAM);

};