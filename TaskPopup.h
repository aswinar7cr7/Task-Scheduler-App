//TaskPopup.h -> Headerfile of dialog box that should popup at the execution start time
#pragma once
#include "afxdialogex.h"

// CTaskPopup dialog

class CTaskPopup : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskPopup)

public:
	CTaskPopup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTaskPopup();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POPUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	// variable of popup text
	CString m_PopupText;
	afx_msg void OnEnChangePopupMsg();

	// variable for duration
	int m_Duration;

	// method to set duration
	void SetDuration(int duration);
};