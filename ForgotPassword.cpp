// ForgotPassword.cpp
#include "pch.h"
#include "TaskSchedulerApp.h"
#include "afxdialogex.h"
#include "ForgotPassword.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <tuple> // Include the tuple header

using namespace std;

// CForgotPassword dialog

IMPLEMENT_DYNAMIC(CForgotPassword, CDialogEx)

CForgotPassword::CForgotPassword(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_ForgotPasswordBox, pParent)
    , m_sNameInFpass(_T(""))
    , m_sNewPassInFpass(_T(""))
    , m_sUserNameInFpass(_T(""))
    , m_nCnfPassInFPass(_T(""))
{
}

CForgotPassword::~CForgotPassword()
{
}

void CForgotPassword::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_NAME_IN_FPASS, m_sNameInFpass);
    DDX_Text(pDX, IDC_NEW_PASS_IN_FPASS, m_sNewPassInFpass);
    DDX_Text(pDX, IDC_USER_N_IN_FPASS, m_sUserNameInFpass);
    DDX_Text(pDX, IDC_CONFIRM_PASS_IN_FPASS, m_nCnfPassInFPass);
}

BEGIN_MESSAGE_MAP(CForgotPassword, CDialogEx)
    ON_EN_CHANGE(IDC_NAME_IN_FPASS, &CForgotPassword::OnEnChangeNameInFpass)
    ON_EN_CHANGE(IDC_USER_N_IN_FPASS, &CForgotPassword::OnEnChangeUserNInFpass)
    ON_EN_CHANGE(IDC_NEW_PASS_IN_FPASS, &CForgotPassword::OnEnChangeNewPassInFpass)
    ON_BN_CLICKED(IDOK, &CForgotPassword::OnBnClickedOk)
    ON_EN_CHANGE(IDC_CONFIRM_PASS_IN_FPASS, &CForgotPassword::OnEnChangeConfirmPassInFpass)
END_MESSAGE_MAP()

vector<tuple<string, string, string>> userData; // Use tuple to store name, username, and password

void LoadUserDataForForgot()
{
    userData.clear();// Empty the current list
    ifstream file("users.txt"); // Open the user database file
    string line, name, username, password;

    while (getline(file, line)) // Read each line
    {
        // Split the line by commas
        stringstream ss(line);
        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, name, ',');

        // Add to our list
        userData.push_back(make_tuple(name, username, password));
    }
    file.close();
}

bool CheckUserExists(const string& username)
{
    LoadUserDataForForgot(); // Refresh data from file
    for (const auto& user : userData)  // Check each user
    {
        if (std::get<1>(user) == username) // Access the username using std::get<1> // Compare usernames
        {
            return true;  // Found match!
        }
    }
    return false; //No match Found
}

void UpdatePassword(const string& username, const string& newPassword)
{

    // Find user and update password
    for (auto& user : userData)
    {
        if (std::get<1>(user) == username) // Access the username using std::get<1>
        {
            std::get<2>(user) = newPassword; // Update the password using std::get<2> // Set new password Updates their password (position 2 in the tuple)
        }
    }
    // Save all changes back to file
    ofstream file("users.txt");
    for (const auto& user : userData)
    {
        file << std::get<1>(user) << "," << std::get<2>(user) << "," << std::get<0>(user) << "\n";
    }
    file.close();
}

void CForgotPassword::LogEvent(const CString& event) {
    std::ofstream logFile("log_file.txt", std::ios::app); // Open file for appending
    if (logFile.is_open()) {
        CStringA eventStr(event); // Convert to ASCII  - The conversion ensures the text is saved in a universally readable format
        logFile << (LPCSTR)eventStr << std::endl;
        logFile.close();
    }
    else {
        AfxMessageBox(_T("Unable to open log file for writing!"));
    }
}

void CForgotPassword::OnEnChangeNameInFpass()
{
    // TODO: Add your control notification handler code here
}

void CForgotPassword::OnEnChangeUserNInFpass()
{
    // TODO: Add your control notification handler code here
}

void CForgotPassword::OnEnChangeNewPassInFpass()
{
    // TODO: Add your control notification handler code here
}

void CForgotPassword::OnBnClickedOk() {
    CString cUsername, cNewPassword, cName, cConfirmPassword;
    GetDlgItemText(IDC_USER_N_IN_FPASS, cUsername);
    GetDlgItemText(IDC_NEW_PASS_IN_FPASS, cNewPassword);
    GetDlgItemText(IDC_NAME_IN_FPASS, cName);
    GetDlgItemText(IDC_CONFIRM_PASS_IN_FPASS, cConfirmPassword);

    if (cUsername.IsEmpty() || cNewPassword.IsEmpty() || cName.IsEmpty() || cConfirmPassword.IsEmpty()) {
        AfxMessageBox(_T("All fields are required!"));
        return;
    }

    if (cNewPassword != cConfirmPassword) {
        AfxMessageBox(_T("Passwords do not match!"));
        return;
    }

    // Password complexity validation
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (int i = 0; i < cNewPassword.GetLength(); i++) {
        TCHAR ch = cNewPassword[i];
        if (iswupper(ch)) hasUpper = true;
        else if (iswlower(ch)) hasLower = true;
        else if (iswdigit(ch)) hasDigit = true;
        else if (iswpunct(ch)) hasSpecial = true;
    }

    if (!hasUpper || !hasLower || !hasDigit || !hasSpecial) {
        AfxMessageBox(_T("Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special character."));
        return;
    }

    string username = CStringA(cUsername);
    string newPassword = CStringA(cNewPassword);
    string name = CStringA(cName);

    // Load user data and check if the combination of username and name exists
    ifstream file("users.txt");
    bool userExists = false;
    vector<string> lines;
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileUsername, filePassword, fileName;
            getline(ss, fileUsername, ',');
            getline(ss, filePassword, ',');
            getline(ss, fileName, ',');

            if (fileUsername == username && fileName == name) {
                userExists = true;
                line = fileUsername + "," + newPassword + "," + fileName; // Update password
            }
            lines.push_back(line);
        }
        file.close();
    }

    if (userExists) {
        ofstream outFile("users.txt");
        for (const auto& line : lines) {
            outFile << line << "\n";
        }
        outFile.close();

        AfxMessageBox(_T("Password changed successfully!"));

        // Log the password change event
        CString logEntry;
        logEntry.Format(_T("Password Changed: Username: %s"), cUsername);
        LogEvent(logEntry);
    }
    else {
        AfxMessageBox(_T("Name or username doesn’t exist!"));
    }

    EndDialog(IDCANCEL);
    CDialogEx::OnOK();
}
void CForgotPassword::OnEnChangeConfirmPassInFpass()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}
