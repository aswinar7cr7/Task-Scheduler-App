//TaskManager.h - Headerfile
#pragma once
#include "afxdialogex.h"
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <fstream>
#include "TaskPopup.h" // Include TaskPopup header

struct Task {
    CString name;
    CString content;
    COleDateTime startTime;
    COleDateTime endTime;
    CString status;
    bool endedLogged; // Add this flag
};

class CTaskManager : public CDialogEx {
    DECLARE_DYNAMIC(CTaskManager)

public:
    CTaskManager( CWnd* pParent = nullptr);
    virtual ~CTaskManager();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TASK_MANAGER };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
    virtual BOOL OnInitDialog();

private:
    std::vector<Task> taskList;
    std::map<COleDateTime, Task> taskMap;
    std::mutex taskMutex;
    std::thread taskThread;
    bool running;
    int m_selectedIndex; // Add selected index member

    //void OnLoginSuccess(const CString& username);

    void SaveTasksToFile(const CString& username);
    void LoadTasksFromFile(const CString& username);
    void LogEvent(const CString& event);
    void LoadLogFromFile();
    void TaskScheduler();





public:
    CString m_sTaskName;
    CString m_sTaskContent;
    COleDateTime m_StartTime;
    COleDateTime m_EndTime;
    CListCtrl m_TaskList;
    CString m_currentUser; // Add this line

    CTaskManager* m_pTaskManager; // Add this line



   // CListCtrl m_LogList;
    void AddTaskToUI(const Task& task); // Add this line
    void OnLoginSuccess(const CString& username); // Declare OnLoginSuccess as public


    void OnBnClickedAddTask();
    afx_msg void OnBnClickedDeleteTask();

#define WM_SHOW_TASK_POPUP (WM_USER + 1)
    afx_msg LRESULT OnShowTaskPopup(WPARAM wParam, LPARAM lParam);
    afx_msg
        void OnLvnItemchangedTaskList(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBnClickedEditTask();

    //void OnOK();
 
    afx_msg void OnBnClickedClear();
};