
// TaskSchedulerAppView.h : interface of the CTaskSchedulerAppView class
//

#pragma once
#include "TaskManager.h" // Add this line


class CTaskSchedulerAppView : public CFormView
{
protected: // create from serialization only
	CTaskSchedulerAppView() noexcept;
	DECLARE_DYNCREATE(CTaskSchedulerAppView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_TASKSCHEDULERAPP_FORM };
#endif

// Attributes
public:
	CTaskSchedulerAppDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CTaskSchedulerAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNewUser();
	afx_msg void OnBnClickedForgotPassword();
	// variable of User Name in Login page
	CString m_sUserName;
	// variable of password in Login page
	CString m_sPassword;
	afx_msg void OnBnClickedLoginButton();
	void LogEvent(const CString& event);
	//void OnTaskManager();
	afx_msg void OnEnChangeUserName();
	afx_msg void OnEnChangePassword();

	void OnLoginSuccess(const CString& username); // Declare OnLoginSuccess as public

	CTaskManager* m_pTaskManager; // Add this line

};

#ifndef _DEBUG  // debug version in TaskSchedulerAppView.cpp
inline CTaskSchedulerAppDoc* CTaskSchedulerAppView::GetDocument() const
   { return reinterpret_cast<CTaskSchedulerAppDoc*>(m_pDocument); }
#endif

