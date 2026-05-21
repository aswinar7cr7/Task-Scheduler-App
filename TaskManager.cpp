//TaskManager.cpp - implementation file


#include "pch.h"
#include "TaskSchedulerApp.h"
#include "TaskManager.h"
#include "TaskPopup.h" // Include TaskPopup header
#include <sstream>

IMPLEMENT_DYNAMIC(CTaskManager, CDialogEx)

CTaskManager::CTaskManager(CWnd* pParent)
    : CDialogEx(IDD_TASK_MANAGER, pParent), running(true), m_selectedIndex(-1)
{
    COleDateTime currentTime = COleDateTime::GetCurrentTime();
    m_StartTime = currentTime;
    m_EndTime = currentTime;

    taskThread = std::thread(&CTaskManager::TaskScheduler, this);
    LoadTasksFromFile(m_currentUser);
    LoadLogFromFile(); // Load log events from file
}

CTaskManager::~CTaskManager() {
    running = false;
    if (taskThread.joinable()) {
        taskThread.join();
    }
}

void CTaskManager::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TASK_NAME, m_sTaskName);
    DDX_Text(pDX, IDC_TASK_CONTENT, m_sTaskContent);
    DDX_DateTimeCtrl(pDX, IDC_START_TIME, m_StartTime);
    DDX_DateTimeCtrl(pDX, IDC_END_TIME, m_EndTime);
    DDX_Control(pDX, IDC_TASK_LIST, m_TaskList);
    // DDX_Control(pDX, IDC_LOG_LIST, m_LogList);
  
}

BEGIN_MESSAGE_MAP(CTaskManager, CDialogEx)
    ON_BN_CLICKED(IDC_ADD_TASK, &CTaskManager::OnBnClickedAddTask)
    ON_BN_CLICKED(IDC_DELETE_TASK, &CTaskManager::OnBnClickedDeleteTask)
    ON_MESSAGE(WM_SHOW_TASK_POPUP, &CTaskManager::OnShowTaskPopup)
    ON_BN_CLICKED(IDC_EDIT_TASK, &CTaskManager::OnBnClickedEditTask)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_TASK_LIST, &CTaskManager::OnLvnItemchangedTaskList) // Add this line
    
    ON_BN_CLICKED(IDC_CLEAR, &CTaskManager::OnBnClickedClear)
END_MESSAGE_MAP()

BOOL CTaskManager::OnInitDialog() {
    BOOL result = CDialogEx::OnInitDialog(); // Store the return value

    CString debugOutput;
    debugOutput.Format(_T("OnInitDialog for user: %s, CDialogEx::OnInitDialog() result: %d\n"), m_currentUser, result);
    OutputDebugString(debugOutput);

    if (result == FALSE) {
        OutputDebugString(_T("CDialogEx::OnInitDialog() returned FALSE!\n"));
    }

    // Debugging: Check m_TaskList's hWnd
    if (::IsWindow(m_TaskList.m_hWnd)) {
        OutputDebugString(_T("m_TaskList's hWnd is valid.\n"));
    }
    else {
        OutputDebugString(_T("m_TaskList's hWnd is INVALID.\n"));
    }

    LoadTasksFromFile(m_currentUser);

    m_TaskList.InsertColumn(0, _T("Task Name"), LVCFMT_LEFT, 150);
    m_TaskList.InsertColumn(1, _T("Status"), LVCFMT_LEFT, 100);
    m_TaskList.InsertColumn(2, _T("Start Time"), LVCFMT_LEFT, 150);
    m_TaskList.InsertColumn(3, _T("End Time"), LVCFMT_LEFT, 150);

    if (!::IsWindow(m_TaskList.m_hWnd)) { // Check if the window is valid
        AfxMessageBox(_T("List control not initialized properly."));
    }

    LoadTasksFromFile(m_currentUser);

    return result; // Return the result of CDialogEx::OnInitDialog()
}



void CTaskManager::OnBnClickedAddTask() {
    UpdateData(TRUE);
    CString debugOutput;
    debugOutput.Format(_T("Adding task for user: %s\n"), m_currentUser);
    OutputDebugString(debugOutput);

    if (m_sTaskName.IsEmpty() || m_sTaskContent.IsEmpty()) {
        AfxMessageBox(_T("Task Name and Content cannot be empty!"));
        return;
    }

    // Validate and initialize start and end times
    if (m_StartTime.GetStatus() != COleDateTime::valid || m_EndTime.GetStatus() != COleDateTime::valid) {
        AfxMessageBox(_T("Invalid start or end time!"));
        return;
    }

    // Get current time
    COleDateTime currentTime = COleDateTime::GetCurrentTime();

    // Check if start time is in the future
    if (m_StartTime <= currentTime) {
        AfxMessageBox(_T("Start time must be in the after present time!"));
        return;
    }

    // Check if end time is after start time
    if (m_EndTime <= m_StartTime) {
        AfxMessageBox(_T("End time must be after start time!"));
        return;
    }

    std::lock_guard<std::mutex> lock(taskMutex);
    for (const auto& task : taskList) {
        if (task.startTime <= m_StartTime && m_StartTime < task.endTime) {
            AfxMessageBox(_T("Task conflict! Another task is already scheduled at this time."));
            return;
        }
    }

    

    Task newTask = { m_sTaskName, m_sTaskContent, m_StartTime, m_EndTime, _T("Scheduled") };
    taskList.insert(taskList.begin(), newTask);
    taskMap[m_StartTime] = newTask;
    SaveTasksToFile(m_currentUser); // Save the new task

    AddTaskToUI(newTask);

    // Add to Task List
    int index = m_TaskList.InsertItem(0, _T("Task Added: ") + m_sTaskName);

    // Create a consistent log entry
    CString logEntry;
    logEntry.Format(_T("Task Added: %s, Start Time: %s, End Time: %s"),
        m_sTaskName,
        m_StartTime.Format(_T("%Y-%m-%d %H:%M:%S")),
        m_EndTime.Format(_T("%Y-%m-%d %H:%M:%S")));

    // Log to UI and file
    //m_LogList.InsertItem(0, logEntry);
    LogEvent(logEntry); // This will write to the log file

    // Clear input fields
    m_sTaskName.Empty();
    m_sTaskContent.Empty();
    m_StartTime = COleDateTime::GetCurrentTime();
    m_EndTime = COleDateTime::GetCurrentTime();

    UpdateData(FALSE); // Update the dialog controls
}


void CTaskManager::OnBnClickedDeleteTask() {
    int selectedItem = m_TaskList.GetNextItem(-1, LVNI_SELECTED);
    if (selectedItem == -1) {
        AfxMessageBox(_T("No task selected!"));
        return;
    }

    // Get the task name from the selected item BEFORE deleting it
    CString taskName = m_TaskList.GetItemText(selectedItem, 0);

    // Remove "Task Added: " prefix if present
    if (taskName.Find(_T("Task Added: ")) == 0) {
        taskName = taskName.Mid(12);
    }

    std::lock_guard<std::mutex> lock(taskMutex);

    // Find the task in taskList using the task name
    auto it = std::find_if(taskList.begin(), taskList.end(), [&](const Task& task) {
        return task.name == taskName;
        });

    if (it != taskList.end()) {
        // Get the start time of the task
        COleDateTime startTime = it->startTime;

        // Remove the task from taskList
        taskList.erase(it);

        // Remove the task from taskMap
        taskMap.erase(startTime);

        // Delete the item from the list control AFTER removing from taskList and taskMap
        m_TaskList.DeleteItem(selectedItem);

        // Clear the selection
        m_TaskList.SetItemState(-1, 0, LVIS_SELECTED);

        // Save the updated tasks to file
        SaveTasksToFile(m_currentUser);

        // Create a consistent log entry
        CString logEntry;
        logEntry.Format(_T("Task Deleted: %s, Time: %s"),
            taskName,
            COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")));

        // Log to UI and file
        LogEvent(logEntry);

        AfxMessageBox(_T("Dask DELETED!"));
    }
    else {
        AfxMessageBox(_T("Task not found!"));
    }
}


void CTaskManager::TaskScheduler() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        COleDateTime now = COleDateTime::GetCurrentTime();

        std::lock_guard<std::mutex> lock(taskMutex);
        for (auto it = taskMap.begin(); it != taskMap.end();) {
            if (it->first <= now && now < it->second.endTime) {
                // Log Task Execution Start
                if (it->second.status == _T("Scheduled")) {
                    CString execLog;
                    execLog.Format(_T("Task Execution Started: %s, Start Time: %s"),
                        it->second.name, it->second.startTime.Format(_T("%H:%M:%S")));
                    //m_LogList.InsertItem(0, execLog);

                    LogEvent(_T("Task Started: ") + it->second.name + _T(", Start Time: ") + it->second.startTime.Format(_T("%H:%M:%S")));

                    // Send a message to show the popup in the main thread
                    PostMessage(WM_SHOW_TASK_POPUP, 0, (LPARAM)&it->second);

                    it->second.status = _T("Executing");
                }
            }

            if (now >= it->second.endTime) {
                // Log Task End
                CString endLog;
                endLog.Format(_T("Task Ended: %s, End Time: %s"),
                    it->second.name, it->second.endTime.Format(_T("%H:%M:%S")));
                //m_LogList.InsertItem(0, endLog);

                LogEvent(_T("Task Ended: ") + it->second.name + _T(", End Time: ") + it->second.endTime.Format(_T("%H:%M:%S")));
                it = taskMap.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

LRESULT CTaskManager::OnShowTaskPopup(WPARAM wParam, LPARAM lParam) {
    if (!lParam) return 0;

    Task* task = (Task*)lParam;
    if (task) {
        // Make a copy to ensure the task data is safe
        Task popupTask = *task;

        CTaskPopup popupDlg;
        popupDlg.m_PopupText = popupTask.content;
        popupDlg.SetDuration((popupTask.endTime - COleDateTime::GetCurrentTime()).GetTotalSeconds());
        popupDlg.DoModal();
    }
    return 0;
}


void CTaskManager::OnLoginSuccess(const CString& username) {
    m_currentUser = username; // Store the logged-in username
    LoadTasksFromFile(m_currentUser); // Load tasks for the logged-in user
}

void CTaskManager::SaveTasksToFile(const CString& username) { // Added username argument
    CString taskFileName = username + _T("_tasks.txt");
    CString debugOutput;
    debugOutput.Format(_T("Saving tasks to: %s\n"), taskFileName);
    OutputDebugString(debugOutput);
    std::ofstream file((CStringA(taskFileName))); // Extra parentheses

    if (file.is_open()) {
        for (const auto& task : taskList) {
            file << CW2A(task.name) << "|" << CW2A(task.content) << "|"
                << CW2A(task.startTime.Format(_T("%Y-%m-%d %H:%M:%S"))) << "|"
                << CW2A(task.endTime.Format(_T("%Y-%m-%d %H:%M:%S"))) << std::endl;
        }
        file.close();
    }
    else {
        OutputDebugString(_T("Failed to open user task file for writing: ") + taskFileName + _T("\n"));
    }
}

void CTaskManager::AddTaskToUI(const Task& task) {
    if (::IsWindow(m_TaskList.m_hWnd)) {
        int index = m_TaskList.InsertItem(m_TaskList.GetItemCount(), task.name); // Add at the end
        m_TaskList.SetItemText(index, 1, task.status);
        m_TaskList.SetItemText(index, 2, task.startTime.Format(_T("%Y-%m-%d %H:%M:%S")));
        m_TaskList.SetItemText(index, 3, task.endTime.Format(_T("%Y-%m-%d %H:%M:%S")));
    }
}

void CTaskManager::LoadTasksFromFile(const CString& username) {
    CString taskFileName = username + _T("_tasks.txt");
    CString debugOutput;
    debugOutput.Format(_T("Loading tasks from: %s\n"), taskFileName);
    OutputDebugString(debugOutput);

    std::ifstream file((CStringA(taskFileName)));


    if (!file.is_open()) {
        OutputDebugString(_T("Failed to open user task file: ") + taskFileName + _T("\n"));
        taskList.clear(); // Clear the list even if file open fails.
        taskMap.clear();
        if (::IsWindow(m_TaskList.m_hWnd)) {
            m_TaskList.DeleteAllItems();
        }
        return;
    }

    taskList.clear();
    taskMap.clear();
    if (::IsWindow(m_TaskList.m_hWnd)) {
        m_TaskList.DeleteAllItems();
    }

    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        lineCount++;
        std::stringstream ss(line);
        std::string name, content, startTimeStr, endTimeStr;
        std::getline(ss, name, '|');
        std::getline(ss, content, '|');
        std::getline(ss, startTimeStr, '|');
        std::getline(ss, endTimeStr, '|');

        Task task;
        task.name = CString(name.c_str());
        task.content = CString(content.c_str());

        // Debugging output for time strings
        CString debugTimeOutput;
        debugTimeOutput.Format(_T("Line %d: Start Time Str='%s', End Time Str='%s'\n"), lineCount, CString(startTimeStr.c_str()), CString(endTimeStr.c_str()));
        OutputDebugString(debugTimeOutput);

        CString startTimeCString(startTimeStr.c_str());
        CString endTimeCString(endTimeStr.c_str());

        // Trim any whitespace
        startTimeCString.Trim();
        endTimeCString.Trim();

        // Debugging output after Trim
        debugTimeOutput.Format(_T("Line %d: Start Time Str (Trimmed)='%s', End Time Str (Trimmed)='%s'\n"), lineCount, startTimeCString, endTimeCString);
        OutputDebugString(debugTimeOutput);

        COleDateTime startTime, endTime;
        if (!startTime.ParseDateTime(startTimeCString)) {
            AfxMessageBox(_T("Failed to parse start time: ") + startTimeCString);
            OutputDebugString(_T("Failed to parse start time. Task Skipped.\n")); //Debugging
            continue; // Skip this task
        }
        if (!endTime.ParseDateTime(endTimeCString)) {
            AfxMessageBox(_T("Failed to parse end time: ") + endTimeCString);
            OutputDebugString(_T("Failed to parse end time. Task Skipped.\n")); //Debugging
            continue; // Skip this task
        }

        // Debugging output for parsed times
        CString parsedTimeOutput;
        parsedTimeOutput.Format(_T("Line %d: Start Time='%s', End Time='%s'\n"), lineCount, startTime.Format(_T("%Y-%m-%d %H:%M:%S")), endTime.Format(_T("%Y-%m-%d %H:%M:%S")));
        OutputDebugString(parsedTimeOutput);

        task.startTime = startTime; // assign the parsed time
        task.endTime = endTime; // assign the parsed time

        taskList.push_back(task);
        taskMap[startTime] = task;

        AddTaskToUI(task);
    }
    file.close();

    if (::IsWindow(m_TaskList.m_hWnd)) {
        CString itemCountOutput;
        itemCountOutput.Format(_T("Total items in list: %d\n"), m_TaskList.GetItemCount());
        OutputDebugString(itemCountOutput);
    }
    else {
        OutputDebugString(_T("m_TaskList.m_hWnd is invalid.\n"));
    }
}

void CTaskManager::LogEvent(const CString& event) {
    std::ifstream logFile("log_file.txt"); // Open file for reading
    std::string line;
    bool alreadyLogged = false;

    // Convert CString to std::string
    std::string eventStr = CT2A(event);

    // Check if event is already logged
    if (logFile.is_open()) {
        while (std::getline(logFile, line)) {
            if (line.find(eventStr) != std::string::npos) {
                alreadyLogged = true;
                break;
            }
        }
        logFile.close();
    }

    // Only log if not already logged
    if (!alreadyLogged) {
        std::ofstream logFileOut("log_file.txt", std::ios::app); // Open file for appending
        if (logFileOut.is_open()) {
            logFileOut << eventStr << std::endl;
            logFileOut.close();
        }
    }
}


void CTaskManager::LoadLogFromFile() {
    std::ifstream file("log_file.txt");
    if (!file.is_open()) {
        // File doesn't exist yet, but that's okay for a new installation
        return;
    }

    std::vector<CString> logLines;
    std::string line;
    
    // Read all lines from the file
    while (std::getline(file, line)) {
        logLines.push_back(CString(line.c_str()));
    }
    
    // Add lines to the list control in reverse order (newest on top)
    /*if (::IsWindow(m_LogList.m_hWnd)) {
        for (auto it = logLines.rbegin(); it != logLines.rend(); ++it) {
            m_LogList.InsertItem(m_LogList.GetItemCount(), *it);
        }
    }*/
}

void CTaskManager::OnLvnItemchangedTaskList(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (pNMLV->uChanged & LVIF_STATE && (pNMLV->uNewState & LVIS_SELECTED)) {
        int selectedIndex = pNMLV->iItem;

        if (selectedIndex >= 0 && selectedIndex < taskList.size()) {
            Task& selectedTask = taskList[selectedIndex];

            // Debugging output for selected task times
            CString selectedTimeOutput;
            selectedTimeOutput.Format(_T("Selected Task (taskList): Start='%s', End='%s'\n"), selectedTask.startTime.Format(_T("%Y-%m-%d %H:%M:%S")), selectedTask.endTime.Format(_T("%Y-%m-%d %H:%M:%S")));
            OutputDebugString(selectedTimeOutput);

            m_sTaskName = selectedTask.name;
            m_sTaskContent = selectedTask.content;
            m_StartTime = selectedTask.startTime;
            m_EndTime = selectedTask.endTime;
            m_selectedIndex = selectedIndex; // Set m_selectedIndex here

            // Debugging output for assigned times
            CString assignedTimeOutput;
            assignedTimeOutput.Format(_T("Assigned: Start='%s', End='%s'\n"), m_StartTime.Format(_T("%Y-%m-%d %H:%M:%S")), m_EndTime.Format(_T("%Y-%m-%d %H:%M:%S")));
            OutputDebugString(assignedTimeOutput);

            UpdateData(FALSE); // Update the dialog controls

            // Debugging output after UpdateData
            CString updatedTimeOutput;
            updatedTimeOutput.Format(_T("Updated (Dialog): Start='%s', End='%s'\n"), m_StartTime.Format(_T("%Y-%m-%d %H:%M:%S")), m_EndTime.Format(_T("%Y-%m-%d %H:%M:%S")));
            OutputDebugString(updatedTimeOutput);

        }
        else {
            m_selectedIndex = -1;
        }
    }
    else if (pNMLV->uChanged & LVIF_STATE && !(pNMLV->uNewState & LVIS_SELECTED)) {
        m_selectedIndex = -1;
    }

    *pResult = 0;
}

void CTaskManager::OnBnClickedEditTask() {
    // Debugging output
    CString debugOutput;
    debugOutput.Format(_T("Edit Task Clicked: m_selectedIndex=%d\n"), m_selectedIndex);
    OutputDebugString(debugOutput);

    if (m_selectedIndex == -1) {
        AfxMessageBox(_T("No task selected!"));
        return;
    }

    std::lock_guard<std::mutex> lock(taskMutex);

    // Ensure selectedIndex is within bounds
    if (m_selectedIndex < 0 || m_selectedIndex >= m_TaskList.GetItemCount()) {
        AfxMessageBox(_T("Task not found!"));
        return;
    }

    // Get the task name from the selected item
    CString taskName = m_TaskList.GetItemText(m_selectedIndex, 0);
    if (taskName.IsEmpty()) {
        AfxMessageBox(_T("Failed to retrieve task name!"));
        return;
    }

    // Remove "Task Added: " prefix if present
    if (taskName.Find(_T("Task Added: ")) == 0) {
        taskName = taskName.Mid(12); // Length of "Task Added: "
    }

    // Find the task in taskList based on the index
    auto it = taskList.begin() + m_selectedIndex;

    if (it != taskList.end()) {
        UpdateData(TRUE); // Get updated data from the UI

        // Debugging output after UpdateData
        CString debugOutputUpdateData;
        debugOutputUpdateData.Format(_T("Edited: Name='%s', Content='%s', Start='%s', End='%s'\n"),
            m_sTaskName, m_sTaskContent, m_StartTime.Format(_T("%Y-%m-%d %H:%M:%S")), m_EndTime.Format(_T("%Y-%m-%d %H:%M:%S")));
        OutputDebugString(debugOutputUpdateData);

        // Validate and initialize start and end times
        if (m_StartTime.GetStatus() != COleDateTime::valid || m_EndTime.GetStatus() != COleDateTime::valid) {
            AfxMessageBox(_T("Invalid start or end time!"));
            return;
        }

        // Check if the new start time is in the future
        if (m_StartTime <= COleDateTime::GetCurrentTime()) {
            AfxMessageBox(_T("You can only edit upcoming tasks!"));
            return;
        }

        // Remove the old task from taskList and taskMap
        COleDateTime oldStartTime = it->startTime;
        taskMap.erase(oldStartTime);

        // Store old values for logging
        CString oldName = it->name;
        CString oldStartTimeStr = it->startTime.Format(_T("%Y-%m-%d %H:%M:%S"));
        CString oldEndTimeStr = it->endTime.Format(_T("%Y-%m-%d %H:%M:%S"));

        // Update the task in taskList
        it->name = m_sTaskName;
        it->content = m_sTaskContent;
        it->startTime = m_StartTime;
        it->endTime = m_EndTime;

        // Debugging output after taskList update
        CString debugOutputTaskList;
        debugOutputTaskList.Format(_T("Updated taskList: Name='%s', Content='%s', Start='%s', End='%s'\n"),
            it->name, it->content, it->startTime.Format(_T("%Y-%m-%d %H:%M:%S")), it->endTime.Format(_T("%Y-%m-%d %H:%M:%S")));
        OutputDebugString(debugOutputTaskList);

        // Add the updated task to taskMap
        taskMap[m_StartTime] = *it;

        // Save updated task list to file for the current user
        CString filename;
        filename.Format(_T("%s_tasks.txt"), m_currentUser);
        std::wofstream outFile(filename);
        if (outFile.is_open()) {
            for (const auto& task : taskList) {
                CString startStr = task.startTime.Format(_T("%Y-%m-%d %H:%M:%S"));
                CString endStr = task.endTime.Format(_T("%Y-%m-%d %H:%M:%S"));
                outFile << task.name.GetString() << L"|"
                    << task.content.GetString() << L"|"
                    << startStr.GetString() << L"|"
                    << endStr.GetString() << std::endl;
            }
            outFile.close();
        }

        // Update the task list control
        m_TaskList.SetItemText(m_selectedIndex, 0, _T("Task Added: ") + m_sTaskName);

        // Create a detailed log entry
        CString logEntry;
        logEntry.Format(_T("Task Updated: %s (was: %s), New Start: %s, New End: %s"),
            m_sTaskName,
            oldName,
            m_StartTime.Format(_T("%Y-%m-%d %H:%M:%S")),
            m_EndTime.Format(_T("%Y-%m-%d %H:%M:%S")));

        // Log to UI and file
        LogEvent(logEntry); // This will write to the log file

        m_selectedIndex = -1; // Reset selected index after editing

        MessageBox(_T("Task edited successfully!"));
    }
    else {
        AfxMessageBox(_T("Task not found!"));
    }
}


void CTaskManager::OnBnClickedClear()
{
    // TODO: Add your control notification handler code here
    m_sTaskName.Empty();
    m_sTaskContent.Empty();
    m_StartTime = COleDateTime::GetCurrentTime();
    m_EndTime = COleDateTime::GetCurrentTime();

    // Update UI with cleared values
    UpdateData(FALSE);
}
