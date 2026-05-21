
// TaskSchedulerAppView.cpp : implementation of the CTaskSchedulerAppView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TaskSchedulerApp.h"
#endif

#include "TaskSchedulerAppDoc.h"
#include "TaskSchedulerAppView.h"
#include "RegisterDlg.h"
#include "ForgotPassword.h"
#include "TaskManager.h"
#include<fstream>
#include<vector>
#include<sstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
vector<pair<string, string>>

userCredentials;
// CTaskSchedulerAppView

IMPLEMENT_DYNCREATE(CTaskSchedulerAppView, CFormView)

BEGIN_MESSAGE_MAP(CTaskSchedulerAppView, CFormView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTaskSchedulerAppView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_NEW_USER, &CTaskSchedulerAppView::OnBnClickedNewUser)
	ON_BN_CLICKED(IDC_FORGOT_PASSWORD, &CTaskSchedulerAppView::OnBnClickedForgotPassword)
	ON_BN_CLICKED(IDC_LOGIN_BUTTON, &CTaskSchedulerAppView::OnBnClickedLoginButton)
	ON_EN_CHANGE(IDC_USER_NAME, &CTaskSchedulerAppView::OnEnChangeUserName)
	ON_EN_CHANGE(IDC_PASSWORD, &CTaskSchedulerAppView::OnEnChangePassword)
END_MESSAGE_MAP()

// CTaskSchedulerAppView construction/destruction

CTaskSchedulerAppView::CTaskSchedulerAppView() noexcept
	: CFormView(IDD_TASKSCHEDULERAPP_FORM)
	, m_sUserName(_T(""))
	, m_sPassword(_T(""))
{
	// TODO: add construction code here
	//m_pTaskManager = new CTaskManager(); // Create the TaskManager dialog


}

CTaskSchedulerAppView::~CTaskSchedulerAppView()
{
	if (m_pTaskManager) {
		//delete m_pTaskManager; // Clean up the TaskManager dialog
		m_pTaskManager = nullptr;
	}
}

void CTaskSchedulerAppView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_NAME, m_sUserName);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
}

BOOL CTaskSchedulerAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CTaskSchedulerAppView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CTaskSchedulerAppView printing


void CTaskSchedulerAppView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CTaskSchedulerAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTaskSchedulerAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTaskSchedulerAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CTaskSchedulerAppView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

void CTaskSchedulerAppView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTaskSchedulerAppView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CTaskSchedulerAppView diagnostics

#ifdef _DEBUG
void CTaskSchedulerAppView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTaskSchedulerAppView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTaskSchedulerAppDoc* CTaskSchedulerAppView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTaskSchedulerAppDoc)));
	return (CTaskSchedulerAppDoc*)m_pDocument;
}
#endif //_DEBUG

// Load user data from file
void LoadUserData()
{
	userCredentials.clear();
	ifstream file("users.txt");
	string line, username, password;

	while (getline(file, line))
	{
		stringstream ss(line);
		getline(ss, username, ',');
		getline(ss, password, ',');
		userCredentials.push_back({ username, password });
	}
	file.close();
}

// Validate login credentials
bool ValidateUser(const string& username, const string& password)
{
	LoadUserData();
	for (const auto& user : userCredentials)
	{
		if (user.first == username && user.second == password)
		{
			return true;
		}
	}
	return false;
}

// CTaskSchedulerAppView message handlers

void CTaskSchedulerAppView::OnBnClickedNewUser()
{
	CRegisterDlg registerDlg; // Create an instance of the dialog(object)
	registerDlg.DoModal();  // Show dialog as a modal window
	
	// TODO: Add your control notification handler code here
}

// Forgot Password button click event
void CTaskSchedulerAppView::OnBnClickedForgotPassword()
{
	CForgotPassword forgotDlg;
	forgotDlg.DoModal();
}





void CTaskSchedulerAppView::OnEnChangeUserName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CTaskSchedulerAppView::OnEnChangePassword()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
// Login button click event
// Login button click event

void CTaskSchedulerAppView::OnBnClickedLoginButton() {
	CString cUsername, cPassword;
	GetDlgItemText(IDC_USER_NAME, cUsername);
	GetDlgItemText(IDC_PASSWORD, cPassword);

	string username = CStringA(cUsername);
	string password = CStringA(cPassword);

	if (ValidateUser(username, password)) {
		//AfxMessageBox(_T("Login Successful!"));

		// Log the login event
		CString logEntry;
		logEntry.Format(_T("User Login, username: %s"), cUsername);
		LogEvent(logEntry);

		// Create and show TaskManager as a modal dialog
		CTaskManager taskManagerDlg;
		taskManagerDlg.OnLoginSuccess(cUsername);  // Pass the username
		taskManagerDlg.DoModal();  // This ensures the window is created


		// Clean up the TaskManager dialog
		delete taskManagerDlg;

		// End the application.
		PostQuitMessage(0);
	}
	else {
		AfxMessageBox(_T("Incorrect username or password"));
	}
}


//hai ello

// Define the LogEvent function
void CTaskSchedulerAppView::LogEvent(const CString& event) {
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
