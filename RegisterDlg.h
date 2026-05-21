// RegisterDlg.h
#pragma once
#include "afxdialogex.h"

// CRegisterDlg dialog

class CRegisterDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CRegisterDlg)

public:
    CRegisterDlg(CWnd* pParent = nullptr);   // standard constructor
    virtual ~CRegisterDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_NewUserREG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    // variable of new username in new registration
    CString m_sNewUserName;
    // variable of password of new user registration
    CString m_sPassword;
    afx_msg void OnEnChangeUsernameinnewreg();
    afx_msg void OnEnChangePasswordinnewreg();
    // variable of name in new registration
    CString m_sNameInNewReg;
    afx_msg void OnEnChangeNameInNewReg();
    // variable of Confirm PAssword
    CString m_CnfPass;
    afx_msg void OnEnChangeCfmPassword();
    afx_msg void OnBnClickedOk();

    // Add the LogEvent function declaration
    void LogEvent(const CString& event);
};