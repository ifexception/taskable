// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "mainframe.h"

#include <vector>

#include <sqlite_modern_cpp/errors.h>

#include <wx/aboutdlg.h>
#include <wx/clipbrd.h>
#include <wx/taskbarbutton.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/resources.h"
#include "../common/util.h"
#include "../common/version.h"

#include "../data/taskitemdata.h"
#include "../models/taskitemmodel.h"

#include "../dialogs/taskitemdlg.h"
#include "../dialogs/employerdlg.h"
#include "../dialogs/clientdlg.h"
#include "../dialogs/projectdlg.h"
#include "../dialogs/categorydlg.h"
#include "../dialogs/editlistdlg.h"
#include "../dialogs/stopwatchtaskdlg.h"
#include "../dialogs/checkforupdatedlg.h"
#include "../dialogs/categoriesdlg.h"

#include "../dialogs/weeklytaskviewdlg.h"
#include "../dialogs/meetingsviewdlg.h"

#include "../dialogs/preferencesdlg.h"

#include "../wizards/databaserestorewizard.h"
#include "taskbaricon.h"

#include "../services/databasebackup.h"
#include "../services/databasebackupdeleter.h"

namespace app::frm
{
// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
/* General Event Handlers */
EVT_CLOSE(MainFrame::OnClose)
EVT_ICONIZE(MainFrame::OnIconize)
EVT_SIZE(MainFrame::OnResize)
EVT_TIMER(IDC_DISMISS_INFOBAR_TIMER, MainFrame::OnDismissInfoBar)
/* Main Menu Event Handlers */
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(ids::ID_NEW_ENTRY_TASK, MainFrame::OnNewEntryTask)
EVT_MENU(ids::ID_NEW_TIMED_TASK, MainFrame::OnNewTimedTask)
EVT_MENU(ids::ID_NEW_EMPLOYER, MainFrame::OnNewEmployer)
EVT_MENU(ids::ID_NEW_PROJECT, MainFrame::OnNewProject)
EVT_MENU(ids::ID_NEW_CLIENT, MainFrame::OnNewClient)
EVT_MENU(ids::ID_NEW_CATEGORY, MainFrame::OnNewCategory)
EVT_MENU(ids::ID_WEEKLY_VIEW, MainFrame::OnWeeklyView)
EVT_MENU(ids::ID_MEETINGS_VIEW, MainFrame::OnMeetingsView)
EVT_MENU(ids::ID_EDIT_EMPLOYER, MainFrame::OnEditEmployer)
EVT_MENU(ids::ID_EDIT_CLIENT, MainFrame::OnEditClient)
EVT_MENU(ids::ID_EDIT_PROJECT, MainFrame::OnEditProject)
EVT_MENU(ids::ID_EDIT_CATEGORY, MainFrame::OnEditCategory)
EVT_MENU(ids::ID_PREFERENCES, MainFrame::OnPreferences)
EVT_MENU(ids::ID_STOPWATCH_TASK, MainFrame::OnTaskStopwatch)
EVT_MENU(ids::ID_CHECK_FOR_UPDATE, MainFrame::OnCheckForUpdate)
EVT_MENU(ids::ID_RESTORE_DATABASE, MainFrame::OnRestoreDatabase)
EVT_MENU(ids::ID_BACKUP_DATABASE, MainFrame::OnBackupDatabase)
EVT_MENU(ids::ID_RETURN_TO_CURRENT_DATE, MainFrame::OnReturnToCurrentDate)
/* Frame Control Event Handlers */
EVT_DATE_CHANGED(MainFrame::IDC_GO_TO_DATE, MainFrame::OnDateChanged)
EVT_BUTTON(MainFrame::IDC_FEEDBACK, MainFrame::OnFeedback)
EVT_CHAR_HOOK(MainFrame::OnKeyDown)
/* ListCtrl Control Event Handlers */
EVT_BUTTON(MainFrame::IDC_PREV_DAY, MainFrame::OnPrevDay)
EVT_BUTTON(MainFrame::IDC_NEXT_DAY, MainFrame::OnNextDay)
EVT_LIST_COL_BEGIN_DRAG(MainFrame::IDC_LIST, MainFrame::OnColumnBeginDrag)
EVT_LIST_ITEM_ACTIVATED(MainFrame::IDC_LIST, MainFrame::OnItemDoubleClick)
EVT_LIST_ITEM_RIGHT_CLICK(MainFrame::IDC_LIST, MainFrame::OnItemRightClick)
EVT_MENU(wxID_COPY, MainFrame::OnPopupMenuCopyToClipboard)
EVT_MENU(wxID_EDIT, MainFrame::OnPopupMenuEdit)
EVT_MENU(wxID_DELETE, MainFrame::OnPopupMenuDelete)
/* Uncategorized Event Handlers */
EVT_COMMAND(wxID_ANY, EVT_TASK_ITEM_INSERTED, MainFrame::OnTaskInserted)
EVT_COMMAND(wxID_ANY, EVT_TASK_ITEM_UPDATED, MainFrame::OnTaskUpdated)
EVT_COMMAND(wxID_ANY, EVT_TASK_ITEM_DELETED, MainFrame::OnTaskDeleted)
EVT_COMMAND(wxID_ANY, START_NEW_STOPWATCH_TASK, MainFrame::OnNewStopwatchTaskFromPausedStopwatchTask)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    const wxString& name)
    :wxFrame(
        nullptr, wxID_ANY, common::GetProgramName(), wxDefaultPosition, wxSize(600, 500), wxDEFAULT_FRAME_STYLE, name)
    , pConfig(config)
    , pLogger(logger)
    , pTaskState(std::make_shared<services::TaskStateService>())
    , pTaskStorage(std::make_unique<services::TaskStorage>())
    , pDismissInfoBarTimer(std::make_unique<wxTimer>(this, IDC_DISMISS_INFOBAR_TIMER))
    , pPrevDayBtn(nullptr)
    , pDatePickerCtrl(nullptr)
    , pNextDayBtn(nullptr)
    , pTotalHoursText(nullptr)
    , pListCtrl(nullptr)
    , pStatusBar(nullptr)
    , pInfoBar(nullptr)
    , pTaskBarIcon(nullptr)
    , bHasPendingTaskToResume(false)
    , pFeedbackButton(nullptr)
    , pFeedbackPopupWindow(nullptr)
    , mItemIndex(-1)
    , mSelectedTaskItemId(-1)
// clang-format on
{
}

MainFrame::~MainFrame()
{
    auto size = GetSize();
    pConfig->SetFrameSize(size);

    if (pTaskBarIcon) {
        delete pTaskBarIcon;
    }

    RunDatabaseBackup();
}

bool MainFrame::CreateFrame()
{
    auto configSize = pConfig->GetFrameSize();
    SetSize(configSize);

    bool success = Create();
    SetMinSize(wxSize(850, 580));
    SetIcon(rc::GetProgramIcon());

    if (pConfig->IsBackupEnabled()) {
        svc::DatabaseBackupDeleter dbBackupDeleter(pConfig);
        dbBackupDeleter.Execute();
    }

    pTaskBarIcon = new TaskBarIcon(this, pConfig, pLogger);
    if (pConfig->IsShowInTray()) {
        pTaskBarIcon->SetTaskBarIcon();
    }

    return success;
}

bool MainFrame::Create()
{
    CreateControls();
    DataToControls();

    return true;
}

void MainFrame::CreateControls()
{
    /* Status Bar Control */
    int statusBarWidths[] = { 156, -1, 36 };

    pStatusBar = CreateStatusBar(3);
    SetStatusWidths(3, statusBarWidths);

    SetStatusText(wxT("Ready"), 0);
    SetStatusText(wxString::Format("%d.%d.%d", TASKABLE_MAJOR, TASKABLE_MINOR, TASKABLE_PATCH), 1);

    wxRect statusBarRect;
    pStatusBar->GetFieldRect(2, statusBarRect);

    pFeedbackButton = new wxBitmapButton(pStatusBar,
        IDC_FEEDBACK,
        rc::GetFeedbackIcon(),
        statusBarRect.GetPosition(),
        wxSize(32, 20),
        wxBU_LEFT | wxBU_RIGHT);

    /* File Menu Control */
    auto fileMenu = new wxMenu();

    auto entryTaskMenuItem =
        fileMenu->Append(ids::ID_NEW_ENTRY_TASK, wxT("New &Entry Task\tCtrl-E"), wxT("Create new entry task"));
    entryTaskMenuItem->SetBitmap(rc::GetEntryTaskIcon());

    auto timedTaskMenuItem =
        fileMenu->Append(ids::ID_NEW_TIMED_TASK, wxT("New &Timed Task\tCtrl-T"), wxT("Create new timed task"));
    timedTaskMenuItem->SetBitmap(rc::GetTimedTaskIcon());

    fileMenu->AppendSeparator();

    auto stopwatchMenuItem =
        fileMenu->Append(ids::ID_STOPWATCH_TASK, wxT("Stop&watch\tCtrl-W"), wxT("Start task stopwatch"));
    stopwatchMenuItem->SetBitmap(rc::GetStopwatchIcon());

    fileMenu->AppendSeparator();
    fileMenu->Append(ids::ID_NEW_EMPLOYER, wxT("New &Employer"), wxT("Create new employer"));
    fileMenu->Append(ids::ID_NEW_CLIENT, wxT("New &Client"), wxT("Create new client"));
    fileMenu->Append(ids::ID_NEW_PROJECT, wxT("New &Project"), wxT("Create new project"));
    fileMenu->Append(ids::ID_NEW_CATEGORY, wxT("New C&ategory"), wxT("Create new category"));
    fileMenu->AppendSeparator();
    auto fileViewMenu = new wxMenu();
    fileViewMenu->Append(ids::ID_WEEKLY_VIEW, wxT("Week View"));
    fileViewMenu->Append(ids::ID_MEETINGS_VIEW, wxT("Meetings View"));
    fileMenu->AppendSubMenu(fileViewMenu, wxT("View"));
    fileMenu->AppendSeparator();
    auto exitMenuItem = fileMenu->Append(wxID_EXIT, wxT("Exit"), wxT("Exit the application"));
    exitMenuItem->SetBitmap(rc::GetQuitIcon());

    /* Edit Menu Control */
    auto editMenu = new wxMenu();
    editMenu->Append(ids::ID_EDIT_EMPLOYER, wxT("Edit &Employer"), wxT("Select a employer to edit"));
    editMenu->Append(ids::ID_EDIT_CLIENT, wxT("Edit &Client"), wxT("Select a client to edit"));
    editMenu->Append(ids::ID_EDIT_PROJECT, wxT("Edit &Project"), wxT("Select a project to edit"));
    editMenu->Append(ids::ID_EDIT_CATEGORY, wxT("Edit C&ategory"), wxT("Select a category to edit"));
    editMenu->AppendSeparator();
    auto preferencesMenuItem =
        editMenu->Append(ids::ID_PREFERENCES, wxT("&Preferences\tCtrl-P"), wxT("Edit application preferences"));
    preferencesMenuItem->SetBitmap(rc::GetSettingsIcon());

    /* Export Menu Control */
    // auto exportMenu = new wxMenu();

    /* Tools Menu Control */
    auto toolsMenu = new wxMenu();
    auto restoreMenuItem = toolsMenu->Append(
        ids::ID_RESTORE_DATABASE, wxT("Restore Database"), wxT("Restore database to a previous point"));
    restoreMenuItem->SetBitmap(rc::GetDatabaseRestoreIcon());
    auto backupMenuItem = toolsMenu->Append(
        ids::ID_BACKUP_DATABASE, wxT("Backup Database"), wxT("Backup database at the current snapshot"));
    backupMenuItem->SetBitmap(rc::GetDatabaseBackupIcon());

    /* Help Menu Control */
    wxMenu* helpMenu = new wxMenu();
    auto helpMenuItem = helpMenu->Append(wxID_ABOUT);
    helpMenuItem->SetBitmap(rc::GetAboutIcon());
    auto checkUpdateMenuItem = helpMenu->Append(
        ids::ID_CHECK_FOR_UPDATE, wxT("Check for update"), wxT("Check if an update is available for application"));
    checkUpdateMenuItem->SetBitmap(rc::GetCheckForUpdateIcon());

    /* Menu Bar */
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("File"));
    menuBar->Append(editMenu, wxT("Edit"));
    // menuBar->Append(exportMenu, wxT("Export"));
    menuBar->Append(toolsMenu, wxT("Tools"));
    menuBar->Append(helpMenu, wxT("Help"));

    SetMenuBar(menuBar);

    /* Accelerator Table */
    wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CTRL, (int) 'N', ids::ID_NEW_ENTRY_TASK);
    entries[1].Set(wxACCEL_CTRL, (int) 'T', ids::ID_NEW_TIMED_TASK);
    entries[2].Set(wxACCEL_CTRL, (int) 'P', ids::ID_PREFERENCES);
    entries[3].Set(wxACCEL_CTRL, (int) 'W', ids::ID_STOPWATCH_TASK);
    entries[4].Set(wxACCEL_CTRL, (int) 'G', ids::ID_RETURN_TO_CURRENT_DATE);

    wxAcceleratorTable table(ARRAYSIZE(entries), entries);
    SetAcceleratorTable(table);

    /* Main Controls */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this);
    mainPanel->SetSizer(mainSizer);

    /* InfoBar Control */
    pInfoBar = new wxInfoBar(mainPanel, wxID_ANY);
    mainSizer->Add(pInfoBar, wxSizerFlags().Expand());

    /* Utilities Panel and Controls*/
    auto utilPanel = new wxPanel(mainPanel);
    mainSizer->Add(utilPanel);
    auto utilSizer = new wxBoxSizer(wxHORIZONTAL);
    utilPanel->SetSizer(utilSizer);

    pPrevDayBtn = new wxButton(utilPanel, IDC_PREV_DAY, wxT("<"), wxDefaultPosition, wxSize(32, 26));
    pPrevDayBtn->SetToolTip(wxT("Go to the previous day"));
    utilSizer->Add(pPrevDayBtn, common::sizers::ControlDefault);

    pDatePickerCtrl = new wxDatePickerCtrl(
        utilPanel, IDC_GO_TO_DATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pDatePickerCtrl->SetToolTip(wxT("Select a date to navigate to"));
    utilSizer->Add(pDatePickerCtrl, common::sizers::ControlDefault);

    pNextDayBtn = new wxButton(utilPanel, IDC_NEXT_DAY, wxT(">"), wxDefaultPosition, wxSize(32, 26));
    pNextDayBtn->SetToolTip(wxT("Go to the next day"));
    utilSizer->Add(pNextDayBtn, common::sizers::ControlDefault);

    pTotalHoursText = new wxStaticText(utilPanel, IDC_HOURS_TEXT, wxT("Total Hours: %d"));
    pTotalHoursText->SetToolTip(wxT("Indicates the total hours spent on tasks for the selected day"));
    utilSizer->AddStretchSpacer();
    utilSizer->Add(pTotalHoursText, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    /* List Control Panel */
    auto listPanel = new wxPanel(mainPanel);
    mainSizer->Add(listPanel, 1, wxEXPAND);
    auto listSizer = new wxBoxSizer(wxHORIZONTAL);
    listPanel->SetSizer(listSizer);

    /* List Control */
    int listStyle = wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES;
    pListCtrl = new wxListCtrl(listPanel, IDC_LIST, wxDefaultPosition, wxDefaultSize, listStyle);
    pListCtrl->SetFocus();
    listSizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);

    wxListItem projectColumn;
    projectColumn.SetId(0);
    projectColumn.SetText(wxT("Project"));
    pListCtrl->InsertColumn(0, projectColumn);

    wxListItem dateColumn;
    dateColumn.SetAlign(wxLIST_FORMAT_CENTER);
    dateColumn.SetText(wxT("Date"));
    long mask = dateColumn.GetMask();
    mask |= wxLIST_MASK_FORMAT;
    dateColumn.SetId(1);
    dateColumn.SetMask(mask);
    dateColumn.SetWidth(wxLIST_AUTOSIZE);
    pListCtrl->InsertColumn(1, dateColumn);

    wxListItem startTimeColumn;
    startTimeColumn.SetAlign(wxLIST_FORMAT_CENTER);
    startTimeColumn.SetText(wxT("Start"));
    mask = startTimeColumn.GetMask();
    mask |= wxLIST_MASK_FORMAT;
    startTimeColumn.SetId(2);
    startTimeColumn.SetMask(mask);
    startTimeColumn.SetWidth(wxLIST_AUTOSIZE);
    pListCtrl->InsertColumn(2, startTimeColumn);

    wxListItem endTimeColumn;
    endTimeColumn.SetAlign(wxLIST_FORMAT_CENTER);
    endTimeColumn.SetText(wxT("End"));
    mask = endTimeColumn.GetMask();
    mask |= wxLIST_MASK_FORMAT;
    endTimeColumn.SetId(3);
    endTimeColumn.SetMask(mask);
    endTimeColumn.SetWidth(wxLIST_AUTOSIZE);
    pListCtrl->InsertColumn(3, endTimeColumn);

    wxListItem durationColumn;
    durationColumn.SetAlign(wxLIST_FORMAT_CENTER);
    durationColumn.SetId(4);
    mask = durationColumn.GetMask();
    mask |= wxLIST_MASK_FORMAT;
    durationColumn.SetMask(mask);
    durationColumn.SetText(wxT("Duration"));
    durationColumn.SetWidth(wxLIST_AUTOSIZE);
    pListCtrl->InsertColumn(4, durationColumn);

    wxListItem categoryColumn;
    categoryColumn.SetId(5);
    categoryColumn.SetText(wxT("Category"));
    pListCtrl->InsertColumn(5, categoryColumn);

    wxListItem descriptionColumn;
    descriptionColumn.SetId(6);
    descriptionColumn.SetText(wxT("Description"));
    pListCtrl->InsertColumn(6, descriptionColumn);
}

void MainFrame::DataToControls()
{
    CalculateTotalTime();
    FillListControl();
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if (pConfig->IsConfirmOnExit() && event.CanVeto()) {
        int ret = wxMessageBox(
            wxT("Are you sure to exit the application?"), common::GetProgramName(), wxICON_QUESTION | wxYES_NO);
        if (ret == wxNO) {
            event.Veto();
            return;
        }
    } else if (pConfig->IsCloseToTray() && pConfig->IsShowInTray() && event.CanVeto()) {
        Hide();
        MSWGetTaskBarButton()->Hide();
        return;
    }
    event.Skip();
}

void MainFrame::OnIconize(wxIconizeEvent& event)
{
    if (event.IsIconized() && pConfig->IsShowInTray() && pConfig->IsMinimizeToTray()) {
        MSWGetTaskBarButton()->Hide();
    }
}

void MainFrame::OnResize(wxSizeEvent& event)
{
    if (pStatusBar) {
        wxRect statusBarRect;
        pStatusBar->GetFieldRect(2, statusBarRect);
        pFeedbackButton->SetPosition(statusBarRect.GetPosition());
    }

    wxSize frameSize = GetClientSize();
    int width = frameSize.GetWidth();
    if (pListCtrl) {
        pListCtrl->SetColumnWidth(0, width * 0.10); // project
        pListCtrl->SetColumnWidth(5, width * 0.10); // category
        pListCtrl->SetColumnWidth(6, width * 0.40); // description
    }

    event.Skip();
}

void MainFrame::OnDismissInfoBar(wxTimerEvent& event)
{
    pDismissInfoBarTimer->Stop();
    pInfoBar->Dismiss();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetIcon(rc::GetProgramIcon64());
    aboutInfo.SetName(common::GetProgramName());
    aboutInfo.SetVersion(wxString::Format("%d.%d.%d", TASKABLE_MAJOR, TASKABLE_MINOR, TASKABLE_PATCH));
    aboutInfo.SetDescription(wxT("A desktop application to help you manage how you've spent\n"
                                 "your time on tasks during the day by tracking the time\n"
                                 "you've spent on those tasks throughout the day"));
    aboutInfo.SetCopyright("(C) 2018-2020");
    aboutInfo.SetWebSite(wxT("https://github.com/ifexception/taskable"));
    aboutInfo.SetLicence(common::GetLicense());
    aboutInfo.AddDeveloper(wxT("Szymon Welgus"));

    wxAboutBox(aboutInfo);
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnNewEntryTask(wxCommandEvent& event)
{
    wxDateTime date = pDatePickerCtrl->GetValue();
    dlg::TaskItemDialog entryTask(this, pLogger, pConfig, constants::TaskItemTypes::EntryTask, date);
    int retCode = entryTask.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnNewTimedTask(wxCommandEvent& event)
{
    wxDateTime date = pDatePickerCtrl->GetValue();
    dlg::TaskItemDialog timedTask(this, pLogger, pConfig, constants::TaskItemTypes::TimedTask, date);
    int retCode = timedTask.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnNewEmployer(wxCommandEvent& event)
{
    dlg::EmployerDialog newEmployer(this, pLogger);
    int retCode = newEmployer.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnNewClient(wxCommandEvent& event)
{
    dlg::ClientDialog newClient(this, pLogger);
    int retCode = newClient.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    dlg::ProjectDialog newProject(this, pLogger);
    int retCode = newProject.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnNewCategory(wxCommandEvent& event)
{
    dlg::CategoriesDialog categoriesDialog(this, pLogger);
    int retCode = categoriesDialog.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnWeeklyView(wxCommandEvent& event)
{
    dlg::WeeklyTaskViewDialog* weeklyTaskViewDialog = new dlg::WeeklyTaskViewDialog(this, pLogger, pConfig);
    weeklyTaskViewDialog->Show(true);
}

void MainFrame::OnMeetingsView(wxCommandEvent& event)
{
    dlg::MeetingsViewDialog* meetingViewDialog = new dlg::MeetingsViewDialog(this, pLogger, pConfig);
    meetingViewDialog->LaunchModeless();
}

void MainFrame::OnEditEmployer(wxCommandEvent& event)
{
    dlg::EditListDialog employerEdit(this, dlg::DialogType::Employer, pLogger);
    int retCode = employerEdit.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnEditClient(wxCommandEvent& event)
{
    dlg::EditListDialog clientEdit(this, dlg::DialogType::Client, pLogger);
    int retCode = clientEdit.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnEditProject(wxCommandEvent& event)
{
    dlg::EditListDialog projectEdit(this, dlg::DialogType::Project, pLogger);
    int retCode = projectEdit.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnEditCategory(wxCommandEvent& event)
{
    dlg::EditListDialog categoryEdit(this, dlg::DialogType::Category, pLogger);
    int retCode = categoryEdit.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnPreferences(wxCommandEvent& event)
{
    dlg::PreferencesDialog preferences(this, pConfig, pLogger, pTaskBarIcon);
    preferences.ShowModal();
}

void MainFrame::OnTaskStopwatch(wxCommandEvent& event)
{
    dlg::StopwatchTaskDialog stopwatchTask(this, pConfig, pLogger, pTaskState, pTaskBarIcon);
    stopwatchTask.Launch();

    pListCtrl->SetFocus();
}

void MainFrame::OnCheckForUpdate(wxCommandEvent& event)
{
    dlg::CheckForUpdateDialog checkForUpdate(this);
    checkForUpdate.LaunchModal();
}

void MainFrame::OnRestoreDatabase(wxCommandEvent& event)
{
    if (pConfig->IsBackupEnabled()) {
        auto wizard = new wizard::DatabaseRestoreWizard(this, pConfig, pLogger);
        wizard->CenterOnParent();
        wizard->Run();
    } else {
        wxMessageBox(wxT("Error! Backup option is turned off\n"
                         "and database cannot be restored."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
    }
}

void MainFrame::OnBackupDatabase(wxCommandEvent& event)
{
    if (pConfig->IsBackupEnabled()) {
        svc::DatabaseBackup databaseBackup(pConfig, pLogger);
        bool result = databaseBackup.Execute();
        if (result) {
            wxMessageBox(
                wxT("Backup completed successfully!"), common::GetProgramName(), wxOK_DEFAULT | wxICON_INFORMATION);
        } else {
            wxMessageBox(wxT("Backup database operation encountered error(s)!"),
                common::GetProgramName(),
                wxOK_DEFAULT | wxICON_ERROR);
        }
    } else {
        wxMessageBox(
            wxT("Warning! Backup option is turned off"), common::GetProgramName(), wxOK_DEFAULT | wxICON_WARNING);
    }
}

void MainFrame::OnReturnToCurrentDate(wxCommandEvent& WXUNUSED(event))
{
    wxDateTime currentDate = wxDateTime::Now();
    if (currentDate.FormatISODate() == pDatePickerCtrl->GetValue().FormatISODate()) {
        return;
    }

    DateChangedProcedure(currentDate);
}

void MainFrame::OnPrevDay(wxCommandEvent& event)
{
    auto currentDateTime = pDatePickerCtrl->GetValue();
    currentDateTime.Add(wxDateSpan::Days(-1));

    DateChangedProcedure(currentDateTime);
}

void MainFrame::OnDateChanged(wxDateEvent& event)
{
    auto date = event.GetDate();

    DateChangedProcedure(date);
}

void MainFrame::OnNextDay(wxCommandEvent& event)
{
    auto currentDateTime = pDatePickerCtrl->GetValue();
    currentDateTime.Add(wxDateSpan::Days(1));

    DateChangedProcedure(currentDateTime);
}

void MainFrame::OnFeedback(wxCommandEvent& event)
{
    delete pFeedbackPopupWindow;
    pFeedbackPopupWindow = new FeedbackPopupWindow(this);

    wxWindow* btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen(wxPoint(-186, -78));
    wxSize sz = btn->GetSize();
    pFeedbackPopupWindow->Position(pos, sz);
    pFeedbackPopupWindow->Popup(nullptr);
}

void MainFrame::OnKeyDown(wxKeyEvent& event)
{
    auto currentDateTime = pDatePickerCtrl->GetValue();

    if (event.GetKeyCode() == WXK_RIGHT) {
        currentDateTime.Add(wxDateSpan::Days(1));
    }
    if (event.GetKeyCode() == WXK_LEFT) {
        currentDateTime.Add(wxDateSpan::Days(-1));
    }

    DateChangedProcedure(currentDateTime);

    event.Skip();
}

void MainFrame::OnItemDoubleClick(wxListEvent& event)
{
    mItemIndex = event.GetIndex();

    data::TaskItemData data;
    auto taskItemId = event.GetData();
    int taskItemTypeId = 0;
    try {
        taskItemTypeId = data.GetTaskItemTypeIdByTaskItemId(taskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error(
            "Error occured on TaskItemData::GetTaskItemTypeIdByTaskItemId() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    constants::TaskItemTypes type = static_cast<constants::TaskItemTypes>(taskItemTypeId);
    wxDateTime dateContext = pDatePickerCtrl->GetValue();

    dlg::TaskItemDialog editTask(this, pLogger, pConfig, type, true, taskItemId, dateContext);
    int retCode = editTask.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnItemRightClick(wxListEvent& event)
{
    mItemIndex = event.GetIndex();
    mSelectedTaskItemId = event.GetData();

    wxMenu menu;

    menu.Append(wxID_COPY, wxT("&Copy to Clipboard"));
    menu.Append(wxID_EDIT, wxT("&Edit"));
    menu.Append(wxID_DELETE, wxT("&Delete"));

    PopupMenu(&menu);
}

void MainFrame::OnPopupMenuCopyToClipboard(wxCommandEvent& event)
{
    CopyToClipboardProcedure(mItemIndex);

    mItemIndex = -1;
}

void MainFrame::OnPopupMenuEdit(wxCommandEvent& event)
{
    data::TaskItemData data;
    int taskItemTypeId = 0;
    try {
        taskItemTypeId = data.GetTaskItemTypeIdByTaskItemId(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error(
            "Error occured on TaskItemData::GetTaskItemTypeIdByTaskItemId() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }
    constants::TaskItemTypes type = static_cast<constants::TaskItemTypes>(taskItemTypeId);
    wxDateTime dateContext = pDatePickerCtrl->GetValue();

    dlg::TaskItemDialog editTask(this, pLogger, pConfig, type, true, mSelectedTaskItemId, dateContext);
    int retCode = editTask.ShowModal();
    ShowInfoBarMessage(retCode);
}

void MainFrame::OnPopupMenuDelete(wxCommandEvent& event)
{
    data::TaskItemData data;
    try {
        data.Delete(mSelectedTaskItemId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemModel::Delete() - {0:d} : {1}", e.get_code(), e.what());
        ShowInfoBarMessage(ids::ID_ERROR_OCCURED);
        return;
    }

    ShowInfoBarMessage(wxID_OK);

    auto selectedDate = pDatePickerCtrl->GetValue();

    CalculateTotalTime(selectedDate);

    pListCtrl->DeleteItem(mItemIndex);

    mItemIndex = -1;
}

void MainFrame::OnColumnBeginDrag(wxListEvent& event)
{
    if (event.GetColumn() == 1 // date column
        || event.GetColumn() == 2 // start column
        || event.GetColumn() == 3 // end column
        || event.GetColumn() == 4 // duration colum
    ) {
        event.Veto();
    }
}

void MainFrame::OnTaskInserted(wxCommandEvent& event)
{
    auto selectedDate = pDatePickerCtrl->GetValue();

    CalculateTotalTime(selectedDate);

    int id = event.GetId();

    data::TaskItemData taskItemData;
    std::unique_ptr<model::TaskItemModel> taskItem;
    if (id != -1) {
        try {
            taskItem = taskItemData.GetById(id);
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured on TaskItemData::GetById() - {0:d} : {1}", e.get_code(), e.what());
            return;
        }

        int columnIndex = 0;
        int listIndex = pListCtrl->InsertItem(columnIndex++, taskItem->GetProject()->GetDisplayName());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetTask()->GetTaskDate());
        pListCtrl->SetItem(listIndex,
            columnIndex++,
            taskItem->GetStartTime() ? taskItem->GetStartTime()->FormatISOTime() : wxT("N/A"));
        pListCtrl->SetItem(
            listIndex, columnIndex++, taskItem->GetEndTime() ? taskItem->GetEndTime()->FormatISOTime() : wxT("N/A"));
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetDuration());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetCategory()->GetName());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetDescription());

        pListCtrl->SetItemBackgroundColour(listIndex, taskItem->GetCategory()->GetColor());

        pListCtrl->SetItemPtrData(listIndex, static_cast<wxUIntPtr>(taskItem->GetTaskItemId()));
    }
}

void MainFrame::OnTaskUpdated(wxCommandEvent& event)
{
    auto selectedDate = pDatePickerCtrl->GetValue();

    CalculateTotalTime(selectedDate);

    int id = event.GetId();

    data::TaskItemData taskItemData;
    std::unique_ptr<model::TaskItemModel> taskItem;
    try {
        taskItem = taskItemData.GetById(id);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetById() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    int columnIndex = 0;
    pListCtrl->SetItem(mItemIndex, columnIndex++, taskItem->GetProject()->GetDisplayName());
    pListCtrl->SetItem(mItemIndex, columnIndex++, taskItem->GetTask()->GetTaskDate());
    pListCtrl->SetItem(
        mItemIndex, columnIndex++, taskItem->GetStartTime() ? taskItem->GetStartTime()->FormatISOTime() : wxT("N/A"));
    pListCtrl->SetItem(
        mItemIndex, columnIndex++, taskItem->GetEndTime() ? taskItem->GetEndTime()->FormatISOTime() : wxT("N/A"));
    pListCtrl->SetItem(mItemIndex, columnIndex++, taskItem->GetDuration());
    pListCtrl->SetItem(mItemIndex, columnIndex++, taskItem->GetCategory()->GetName());
    pListCtrl->SetItem(mItemIndex, columnIndex++, taskItem->GetDescription());

    pListCtrl->SetItemBackgroundColour(mItemIndex, taskItem->GetCategory()->GetColor());

    pListCtrl->SetItemPtrData(mItemIndex, static_cast<wxUIntPtr>(taskItem->GetTaskItemId()));

    pListCtrl->RefreshItem(mItemIndex);

    mItemIndex = -1;
}

void MainFrame::OnTaskDeleted(wxCommandEvent& event)
{
    auto selectedDate = pDatePickerCtrl->GetValue();

    CalculateTotalTime(selectedDate);

    pListCtrl->DeleteItem(mItemIndex);

    mItemIndex = -1;
}

void MainFrame::OnNewStopwatchTaskFromPausedStopwatchTask(wxCommandEvent& event)
{
    pTaskStorage->Store(pTaskState);
    pTaskState->mTimes.clear();

    dlg::StopwatchTaskDialog stopwatchTask(
        this, pConfig, pLogger, pTaskState, pTaskBarIcon, /* hasPendingPausedTask */ true);
    stopwatchTask.Launch();

    pTaskState->mTimes.clear();
    pTaskStorage->Restore(pTaskState);

    dlg::StopwatchTaskDialog stopwatchPausedTask(this, pConfig, pLogger, pTaskState, pTaskBarIcon);
    stopwatchPausedTask.Relaunch();

    pTaskStorage->mTimes.clear();

    pListCtrl->SetFocus();
}

void MainFrame::CalculateTotalTime(wxDateTime date)
{
    auto dateString = date.FormatISODate();

    data::TaskItemData taskItemData;
    std::vector<wxString> taskDurations;
    try {
        taskDurations = taskItemData.GetHours(dateString);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetHours() - {0:d} : {1}", e.get_code(), e.what());
    }

    wxTimeSpan totalDuration;
    for (auto duration : taskDurations) {
        std::vector<std::string> durationSplit = util::lib::split(duration.ToStdString(), ':');

        wxTimeSpan currentDuration(std::atol(durationSplit[0].c_str()),
            std::atol(durationSplit[1].c_str()),
            (wxLongLong) std::atoll(durationSplit[2].c_str()));

        totalDuration += currentDuration;
    }

    pTotalHoursText->SetLabel(totalDuration.Format(constants::TotalHours));
}

void MainFrame::FillListControl(wxDateTime date)
{
    wxString dateString = date.FormatISODate();

    data::TaskItemData taskItemData;
    std::vector<std::unique_ptr<model::TaskItemModel>> taskItems;
    try {
        taskItems = taskItemData.GetByDate(dateString);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on TaskItemData::GetByDate() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (const auto& taskItem : taskItems) {
        listIndex = pListCtrl->InsertItem(columnIndex++, taskItem->GetProject()->GetDisplayName());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetTask()->GetTaskDate());
        pListCtrl->SetItem(listIndex,
            columnIndex++,
            taskItem->GetStartTime() ? taskItem->GetStartTime()->FormatISOTime() : wxT("N/A"));
        pListCtrl->SetItem(
            listIndex, columnIndex++, taskItem->GetEndTime() ? taskItem->GetEndTime()->FormatISOTime() : wxT("N/A"));
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetDuration());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetCategory()->GetName());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem->GetDescription());

        pListCtrl->SetItemBackgroundColour(listIndex, taskItem->GetCategory()->GetColor());

        pListCtrl->SetItemPtrData(listIndex, static_cast<wxUIntPtr>(taskItem->GetTaskItemId()));

        columnIndex = 0;
    }
}

bool MainFrame::RunDatabaseBackup()
{
    if (pConfig->IsBackupEnabled()) {
        svc::DatabaseBackup dbBackup(pConfig, pLogger);
        return dbBackup.Execute();
    }

    return true;
}

void MainFrame::ShowInfoBarMessage(int modalRetCode)
{
    if (modalRetCode == wxID_OK) {
        pInfoBar->ShowMessage(constants::OperationCompletedSuccessfully, wxICON_INFORMATION);
    } else {
        pInfoBar->ShowMessage(constants::OperationEncounteredErrors, wxICON_ERROR);
    }

    pDismissInfoBarTimer->Start(1500);
}

void MainFrame::DateChangedProcedure(wxDateTime dateTime)
{
    pListCtrl->DeleteAllItems();
    pDatePickerCtrl->SetValue(dateTime);

    CalculateTotalTime(dateTime);
    FillListControl(dateTime);

    pListCtrl->SetFocus();
}

void MainFrame::CopyToClipboardProcedure(long itemIndex)
{
    auto canOpen = wxTheClipboard->Open();
    if (canOpen) {
        wxListItem listItem;
        listItem.m_itemId = itemIndex;
        listItem.m_col = 6;
        listItem.m_mask = wxLIST_MASK_TEXT;
        pListCtrl->GetItem(listItem);

        auto textData = new wxTextDataObject(listItem.GetText());
        wxTheClipboard->SetData(textData);
        wxTheClipboard->Close();
    }
}
} // namespace app::frm
