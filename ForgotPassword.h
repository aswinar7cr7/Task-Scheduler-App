// ForgotPassword.h
#pragma once
#include "afxdialogex.h"

// CForgotPassword dialog

class CForgotPassword : public CDialogEx
{
    DECLARE_DYNAMIC(CForgotPassword)

public:
    CForgotPassword(CWnd* pParent = nullptr);   // standard constructor
    virtual ~CForgotPassword();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ForgotPasswordBox };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnChangeNameInFpass();
    afx_msg void OnEnChangeUserNInFpass();
    afx_msg void OnEnChangeNewPassInFpass();
    // variable of name in forgot password dialogbox
    CString m_sNameInFpass;

    // variable of new password in forgot password dialogbox
    CString m_sNewPassInFpass;
    // variable of Username in forgot password dialog box
    CString m_sUserNameInFpass;
    afx_msg void OnBnClickedOk();

    // Add the LogEvent function declaration
    void LogEvent(const CString& event);
    // variable of confirm password in forgot password box
    CString m_nCnfPassInFPass;
    afx_msg void OnEnChangeConfirmPassInFpass();
};