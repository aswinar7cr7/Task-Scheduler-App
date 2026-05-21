// RegisterDlg.cpp
#include "pch.h"
#include "TaskSchedulerApp.h"
#include "afxdialogex.h"
#include "RegisterDlg.h"
#include <fstream>
#include <afxwin.h>
#include <string>
using namespace std;

// CRegisterDlg dialog

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG1, pParent)
    , m_sNewUserName(_T(""))
    , m_sPassword(_T(""))
    , m_sNameInNewReg(_T(""))
    , m_CnfPass(_T(""))
{
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_UserNameInNewReg, m_sNewUserName);
    DDX_Text(pDX, IDC_PasswordInNewReg, m_sPassword);
    DDX_Text(pDX, IDC_NAME_IN_NEW_REG, m_sNameInNewReg);
    DDX_Text(pDX, IDC_CFM_PASSWORD, m_CnfPass);
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
    ON_EN_CHANGE(IDC_UserNameInNewReg, &CRegisterDlg::OnEnChangeUsernameinnewreg)
    ON_EN_CHANGE(IDC_PasswordInNewReg, &CRegisterDlg::OnEnChangePasswordinnewreg)
    ON_EN_CHANGE(IDC_NAME_IN_NEW_REG, &CRegisterDlg::OnEnChangeNameInNewReg)
    ON_EN_CHANGE(IDC_CFM_PASSWORD, &CRegisterDlg::OnEnChangeCfmPassword)
    ON_BN_CLICKED(IDOK, &CRegisterDlg::OnBnClickedOk)
END_MESSAGE_MAP()

//creating user.txt
void SaveUserData(const CString& cName, const CString& cUsername, const CString& cPassword) {
    ofstream file("users.txt", ios::app);
    if (file.is_open()) {
        CStringA name(cName);
        CStringA username(cUsername);
        CStringA password(cPassword);
        CStringA taskFileName(cUsername + _T("_tasks.txt")); // creating User-specific task file

        file << (LPCSTR)username << "," << (LPCSTR)password << "," << (LPCSTR)name << "," << (LPCSTR)taskFileName << "\n";
        file.close();
    }
    else {
        AfxMessageBox(_T("Unable to open file for writing!"));
    }
}


void CRegisterDlg::LogEvent(const CString& event) {
    std::ofstream logFile("log_file.txt", std::ios::app); // Open file for appending
    if (logFile.is_open()) {
        CStringA eventStr(event);
        logFile << (LPCSTR)eventStr << std::endl;
        logFile.close();
    }
    else {
        AfxMessageBox(_T("Unable to open log file for writing!"));
    }
}

void CRegisterDlg::OnEnChangeUsernameinnewreg()
{
    // TODO: Add your control notification handler code here
}

void CRegisterDlg::OnEnChangePasswordinnewreg()
{
    // TODO: Add your control notification handler code here
}

void CRegisterDlg::OnEnChangeNameInNewReg()
{
    // TODO: Add your control notification handler code here
}

void CRegisterDlg::OnEnChangeCfmPassword()
{
    // TODO: Add your control notification handler code here
}

void CRegisterDlg::OnBnClickedOk() {
    CString cName, cUsername, cPassword, cConfirmPassword;
    GetDlgItemText(IDC_NAME_IN_NEW_REG, cName);
    GetDlgItemText(IDC_UserNameInNewReg, cUsername);
    GetDlgItemText(IDC_PasswordInNewReg, cPassword);
    GetDlgItemText(IDC_CFM_PASSWORD, cConfirmPassword);

    if (cUsername.IsEmpty() || cPassword.IsEmpty() || cName.IsEmpty() || cConfirmPassword.IsEmpty()) {
        AfxMessageBox(_T("All fields are required!"));
        return;
    }

    if (cPassword != cConfirmPassword) {
        AfxMessageBox(_T("Passwords do not match!"));
        return;
    }

    // Password complexity validation
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (int i = 0; i < cPassword.GetLength(); i++) {
        TCHAR ch = cPassword[i];
        if (iswupper(ch)) hasUpper = true;
        else if (iswlower(ch)) hasLower = true;
        else if (iswdigit(ch)) hasDigit = true;
        else if (iswpunct(ch)) hasSpecial = true;

    }

    if (!hasUpper || !hasLower || !hasDigit || !hasSpecial) {
        AfxMessageBox(_T("Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special character."));
        return;
    }

    // Check if the username is unique
    ifstream file("users.txt");
    if (file.is_open()) {
        CStringA username(cUsername);
        string line;
        while (getline(file, line)) {
            if (line.find((LPCSTR)username) == 0) {
                AfxMessageBox(_T("Username already exists! Please choose a different username."));
                file.close();
                return;
            }
        }
        file.close();
    }

    // Create the user's task file
    CString taskFileName = cUsername + _T("_tasks.txt");
    ofstream userTaskFile((CStringA(taskFileName))); 
    if (!userTaskFile.is_open()) {
        AfxMessageBox(_T("Failed to create user task file."));
        return; // Return if the user task file cannot be created.
    }
    userTaskFile.close();

    // Save user data, including task file name
    ofstream userFile("users.txt", ios::app);
    if (userFile.is_open()) {
        CStringA name(cName);
        CStringA username(cUsername);
        CStringA password(cPassword);
        CStringA taskFileToSave(taskFileName);

        userFile << (LPCSTR)username << "," << (LPCSTR)password << "," << (LPCSTR)name << "," << (LPCSTR)taskFileToSave << "\n";
        userFile.close();
    }
    else {
        AfxMessageBox(_T("Unable to open users.txt for writing!"));
        return; // Return if the users file cannot be opened.
    }

    AfxMessageBox(_T("User Registered Successfully!"));

    CString logEntry;
    logEntry.Format(_T("User Registered: %s, Username: %s"), cName, cUsername);
    LogEvent(logEntry);

    EndDialog(IDCANCEL);
    CDialogEx::OnOK();
}