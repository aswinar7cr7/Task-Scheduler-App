//TaskPopup -> implimentaion file
#include "pch.h"
#include "TaskSchedulerApp.h"
#include "afxdialogex.h"
#include "TaskPopup.h"

// CTaskPopup dialog

IMPLEMENT_DYNAMIC(CTaskPopup, CDialogEx)

CTaskPopup::CTaskPopup(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POPUP, pParent)
	, m_PopupText(_T(""))
	, m_Duration(0)
{

}

CTaskPopup::~CTaskPopup()
{
}

void CTaskPopup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_PopupText);
}

BEGIN_MESSAGE_MAP(CTaskPopup, CDialogEx)
	ON_EN_CHANGE(IDC_POPUP_MSG, &CTaskPopup::OnEnChangePopupMsg)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTaskPopup message handlers

void CTaskPopup::OnEnChangePopupMsg()
{
	// TODO: Add your control notification handler code here
}

void CTaskPopup::SetDuration(int duration)
{
	m_Duration = duration;
}

BOOL CTaskPopup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set a timer to close the dialog after the specified duration
	SetTimer(1, m_Duration * 1000, nullptr);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTaskPopup::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		EndDialog(IDOK);
	}

	CDialogEx::OnTimer(nIDEvent);
}