//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "mainframe.h"

#include <vector>

#include <sqlite_modern_cpp/errors.h>
#include <wx/aboutdlg.h>
#include <wx/clipbrd.h>
#include <wx/taskbarbutton.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../common/version.h"
#include "../dialogs/taskitemdlg.h"
#include "../dialogs/employerdlg.h"
#include "../dialogs/clientdlg.h"
#include "../dialogs/projectdlg.h"
#include "../dialogs/categorydlg.h"
#include "../dialogs/editlistdlg.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/stopwatchtaskdlg.h"
#include "../dialogs/checkforupdatedlg.h"
#include "../services/db_service.h"
#include "../wizards/setupwizard.h"
#include "../dialogs/categoriesdlg.h"
#include "taskbaricon.h"

namespace app::frame
{
// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(ids::ID_NEW_ENTRY_TASK, MainFrame::OnNewEntryTask)
EVT_MENU(ids::ID_NEW_TIMED_TASK, MainFrame::OnNewTimedTask)
EVT_MENU(ids::ID_NEW_EMPLOYER, MainFrame::OnNewEmployer)
EVT_MENU(ids::ID_NEW_PROJECT, MainFrame::OnNewProject)
EVT_MENU(ids::ID_NEW_CLIENT, MainFrame::OnNewClient)
EVT_MENU(ids::ID_NEW_CATEGORY, MainFrame::OnNewCategory)
EVT_MENU(ids::ID_EDIT_EMPLOYER, MainFrame::OnEditEmployer)
EVT_MENU(ids::ID_EDIT_CLIENT, MainFrame::OnEditClient)
EVT_MENU(ids::ID_EDIT_PROJECT, MainFrame::OnEditProject)
EVT_MENU(ids::ID_EDIT_CATEGORY, MainFrame::OnEditCategory)
EVT_MENU(ids::ID_SETTINGS, MainFrame::OnSettings)
EVT_MENU(ids::ID_STOPWATCH_TASK, MainFrame::OnTaskStopwatch)
EVT_MENU(ids::ID_CHECK_FOR_UPDATE, MainFrame::OnCheckForUpdate)
EVT_LIST_ITEM_ACTIVATED(MainFrame::IDC_LIST, MainFrame::OnItemDoubleClick)
EVT_LIST_ITEM_RIGHT_CLICK(MainFrame::IDC_LIST, MainFrame::OnItemRightClick)
EVT_COMMAND(wxID_ANY, EVT_TASK_ITEM_INSERTED, MainFrame::OnTaskInserted)
EVT_COMMAND(wxID_ANY, START_NEW_STOPWATCH_TASK, MainFrame::OnNewStopwatchTaskFromPausedStopwatchTask)
EVT_ICONIZE(MainFrame::OnIconize)
EVT_DATE_CHANGED(MainFrame::IDC_GO_TO_DATE, MainFrame::OnDateChanged)
EVT_SIZE(MainFrame::OnResize)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    const wxString& name)
    : wxFrame(nullptr, wxID_ANY, common::GetProgramName(), wxDefaultPosition, wxSize(600, 500), wxDEFAULT_FRAME_STYLE, name)
    , pLogger(logger)
    , pConfig(config)
    , pTaskState(std::make_shared<services::TaskStateService>())
    , pTaskStorage(std::make_unique<services::TaskStorage>())
    , bHasPendingTaskToResume(false)
    , bHasInitialized(false)
    , pTaskBarIcon(nullptr)
// clang-format on
{
}

MainFrame::~MainFrame()
{
    auto size = GetSize();
    pConfig->SetFrameSize(size);

    if (pTaskBarIcon != nullptr) {
        delete pTaskBarIcon; // TODO Wrap TaskBarIcon in a std::unique_ptr
    }
}

bool MainFrame::RunWizard()
{
    auto wizard = new wizard::SetupWizard(this, pLogger);
    wizard->CenterOnParent();
    bool wizardSetupSuccess = wizard->Run();
    return wizardSetupSuccess;
}

bool MainFrame::CreateFrame()
{
    bool success = Create();
    SetMinClientSize(wxSize(599, 499));
    SetIcon(common::GetProgramIcon());

    auto configSize = pConfig->GetFrameSize();
    SetSize(configSize);

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
    pStatusBar = CreateStatusBar(2);
    SetStatusText(wxT("Ready"));
    SetStatusText(wxString::Format("%d.%d.%d", TASKABLE_MAJOR, TASKABLE_MINOR, TASKABLE_PATCH), 1);

    /* File Menu Control */
    auto fileMenu = new wxMenu();

    auto entryTaskMenuItem =
        fileMenu->Append(ids::ID_NEW_ENTRY_TASK, wxT("New &Entry Task\tCtrl-N"), wxT("Create new entry task"));
    entryTaskMenuItem->SetBitmap(common::GetEntryTaskIcon());

    auto timedTaskMenuItem =
        fileMenu->Append(ids::ID_NEW_TIMED_TASK, wxT("New &Timed Task\tCtrl-T"), wxT("Create new timed task"));
    timedTaskMenuItem->SetBitmap(common::GetTimedTaskIcon());

    fileMenu->AppendSeparator();

    auto stopwatchMenuItem = fileMenu->Append(ids::ID_STOPWATCH_TASK, wxT("&Stopwatch Task\tCtrl-Q"), wxT("Start task stopwatch"));
    stopwatchMenuItem->SetBitmap(common::GetStopwatchIcon());

    fileMenu->AppendSeparator();
    fileMenu->Append(ids::ID_NEW_EMPLOYER, wxT("New &Employer"), wxT("Create new employer"));
    fileMenu->Append(ids::ID_NEW_CLIENT, wxT("New &Client"), wxT("Create new client"));
    fileMenu->Append(ids::ID_NEW_PROJECT, wxT("New &Project"), wxT("Create new project"));
    fileMenu->Append(ids::ID_NEW_CATEGORY, wxT("New C&ategory"), wxT("Create new category"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    /* Edit Menu Control */
    auto editMenu = new wxMenu();
    editMenu->Append(ids::ID_EDIT_EMPLOYER, wxT("Edit &Employer"), wxT("Select a employer to edit"));
    editMenu->Append(ids::ID_EDIT_CLIENT, wxT("Edit &Client"), wxT("Select a client to edit"));
    editMenu->Append(ids::ID_EDIT_PROJECT, wxT("Edit &Project"), wxT("Select a project to edit"));
    editMenu->Append(ids::ID_EDIT_CATEGORY, wxT("Edit C&ategory"), wxT("Select a category to edit"));
    editMenu->AppendSeparator();
    editMenu->Append(ids::ID_SETTINGS, wxT("&Settings\tCtrl-P"), wxT("Edit application settings"));

    /* Export Menu Control */
    auto exportMenu = new wxMenu();

    /* Help Menu Control */
    wxMenu* helpMenu = new wxMenu();
    auto helpMenuItem = helpMenu->Append(wxID_ABOUT);
    helpMenuItem->SetBitmap(common::GetAboutIcon());
    auto checkUpdateMenuItem = helpMenu->Append(
        ids::ID_CHECK_FOR_UPDATE, wxT("Check for update"), wxT("Check if an update is available for application"));
    checkUpdateMenuItem->SetBitmap(common::GetCheckForUpdateIcon());

    /* Menu Bar */
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(exportMenu, wxT("E&xport"));
    menuBar->Append(helpMenu, wxT("&Help"));

    SetMenuBar(menuBar);

    /* Accelerator Table */
    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_CTRL, (int) 'N', ids::ID_NEW_ENTRY_TASK);
    entries[0].Set(wxACCEL_CTRL, (int) 'T', ids::ID_NEW_TIMED_TASK);
    entries[1].Set(wxACCEL_CTRL, (int) 'P', ids::ID_SETTINGS);
    entries[2].Set(wxACCEL_CTRL, (int) 'H', wxID_ABOUT);
    entries[3].Set(wxACCEL_CTRL, (int) 'Q', ids::ID_STOPWATCH_TASK);

    wxAcceleratorTable table(ARRAYSIZE(entries), entries);
    SetAcceleratorTable(table);

    /* Main Controls */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this);
    mainPanel->SetSizer(mainSizer);

    /* Utilities Panel and Controls*/
    auto utilPanel = new wxPanel(mainPanel);
    mainSizer->Add(utilPanel);
    auto utilSizer = new wxBoxSizer(wxHORIZONTAL);
    utilPanel->SetSizer(utilSizer);

    auto gotoText = new wxStaticText(utilPanel, wxID_ANY, wxT("Go To"));
    utilSizer->Add(gotoText, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    pDatePickerCtrl = new wxDatePickerCtrl(
        utilPanel, IDC_GO_TO_DATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pDatePickerCtrl->SetToolTip(wxT("Select a date to navigate to"));
    utilSizer->Add(pDatePickerCtrl, common::sizers::ControlDefault);

    pTotalHoursText = new wxStaticText(utilPanel, IDC_HOURS_TEXT, wxT("Total Hours: %d"));
    pTotalHoursText->SetToolTip(wxT("Indicates the total hours spent on a given task for selected day"));
    utilSizer->AddStretchSpacer();
    utilSizer->Add(pTotalHoursText, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    /* List Panel and Control*/
    auto listPanel = new wxPanel(mainPanel);
    mainSizer->Add(listPanel, 1, wxEXPAND);
    auto listSizer = new wxBoxSizer(wxHORIZONTAL);
    listPanel->SetSizer(listSizer);

    pListCtrl = new wxListCtrl(
        listPanel, IDC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
    pListCtrl->SetFocus();
    listSizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);

    wxListItem projectColumn;
    projectColumn.SetId(0);
    projectColumn.SetText(wxT("Project"));
    pListCtrl->InsertColumn(0, projectColumn);

    wxListItem dateColumn;
    dateColumn.SetId(1);
    dateColumn.SetText(wxT("Date"));
    pListCtrl->InsertColumn(1, dateColumn);

    wxListItem startTimeColumn;
    startTimeColumn.SetId(2);
    startTimeColumn.SetText(wxT("Started"));
    pListCtrl->InsertColumn(2, startTimeColumn);

    wxListItem endTimeColumn;
    endTimeColumn.SetId(3);
    endTimeColumn.SetText(wxT("Ended"));
    pListCtrl->InsertColumn(3, endTimeColumn);

    wxListItem durationColumn;
    durationColumn.SetId(4);
    durationColumn.SetText(wxT("Duration"));
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
    RefreshItems();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetIcon(common::GetProgramIcon());
    aboutInfo.SetName(common::GetProgramName());
    aboutInfo.SetVersion(wxString::Format("%d.%d.%d", TASKABLE_MAJOR, TASKABLE_MINOR, TASKABLE_PATCH));
    aboutInfo.SetDescription(
        wxT("A desktop application to help you manage how you've spent your time on tasks during the day\n"
            " by tracking the time you've spent on those tasks throughout the day"));
    aboutInfo.SetCopyright("(C) 2018-2019");
    aboutInfo.SetWebSite(wxT("https://github.com/ifexception/taskable"));
    aboutInfo.SetLicence(common::GetLicense());
    aboutInfo.AddDeveloper(wxT("Szymon Welgus"));

    wxAboutBox(aboutInfo);
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

void MainFrame::OnNewEntryTask(wxCommandEvent& event)
{
    dialog::TaskItemDialog entryTask(this, pLogger, pConfig, constants::TaskItemTypes::EntryTask);
    entryTask.ShowModal();
}

void MainFrame::OnNewTimedTask(wxCommandEvent& event)
{
    dialog::TaskItemDialog timedTask(this, pLogger, pConfig, constants::TaskItemTypes :: TimedTask);
    timedTask.ShowModal();
}

void MainFrame::OnNewEmployer(wxCommandEvent& event)
{
    dialog::EmployerDialog newEmployer(this, pLogger);
    newEmployer.ShowModal();
}

void MainFrame::OnNewClient(wxCommandEvent& event)
{
    dialog::ClientDialog newClient(this, pLogger);
    newClient.ShowModal();
}

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    dialog::ProjectDialog newProject(this, pLogger);
    newProject.ShowModal();
}

void MainFrame::OnNewCategory(wxCommandEvent& event)
{
    dialog::CategoriesDialog categoriesDialog(this, pLogger);
    categoriesDialog.ShowModal();
}

void MainFrame::OnEditEmployer(wxCommandEvent& event)
{
    dialog::EditListDialog employerEdit(this, dialog::DialogType::Employer, pLogger);
    employerEdit.ShowModal();
}

void MainFrame::OnEditClient(wxCommandEvent& event)
{
    dialog::EditListDialog clientEdit(this, dialog::DialogType::Client, pLogger);
    clientEdit.ShowModal();
}

void MainFrame::OnEditProject(wxCommandEvent& event)
{
    dialog::EditListDialog projectEdit(this, dialog::DialogType::Project, pLogger);
    projectEdit.ShowModal();
}

void MainFrame::OnEditCategory(wxCommandEvent& event)
{
    dialog::EditListDialog categoryEdit(this, dialog::DialogType::Category, pLogger);
    categoryEdit.ShowModal();
}

void MainFrame::OnTaskInserted(wxCommandEvent& event)
{
    pListCtrl->DeleteAllItems();

    auto selectedDate = pDatePickerCtrl->GetValue();

    CalculateTotalTime(selectedDate);
    RefreshItems(selectedDate);
}

void MainFrame::OnItemDoubleClick(wxListEvent& event)
{
    unsigned int taskItemDataPtr = (unsigned int) event.GetData();
    auto taskItemId = (taskItemDataPtr & 0x000000FF);
    auto taskItemTypeId = (taskItemDataPtr & 0x0000FF00) >> 8;
    constants::TaskItemTypes type = static_cast<constants::TaskItemTypes>(taskItemTypeId);
    wxDateTime dateContext = pDatePickerCtrl->GetValue();

    dialog::TaskItemDialog editTask(this, pLogger, pConfig, type, true, taskItemId, dateContext);
    editTask.ShowModal();
}

void MainFrame::OnItemRightClick(wxListEvent& event)
{
    auto canOpen = wxTheClipboard->Open();
    if (canOpen) {
        auto item = event.GetItem();
        wxListItem listItem;
        listItem.m_itemId = item;
        listItem.m_col = 6;
        listItem.m_mask = wxLIST_MASK_TEXT;
        pListCtrl->GetItem(listItem);

        auto textData = new wxTextDataObject(listItem.GetText());
        wxTheClipboard->SetData(textData);
        wxTheClipboard->Close();
    }
}

void MainFrame::OnIconize(wxIconizeEvent& event)
{
    if (event.IsIconized() && pConfig->IsShowInTray() && pConfig->IsMinimizeToTray()) {
        MSWGetTaskBarButton()->Hide();
    }
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    dialog::SettingsDialog settings(this, pConfig);
    settings.ShowModal();
}

void MainFrame::OnTaskStopwatch(wxCommandEvent& event)
{
    dialog::StopwatchTaskDialog stopwatchTask(this, pConfig, pLogger, pTaskState);
    stopwatchTask.Launch();
}

void MainFrame::OnDateChanged(wxDateEvent& event)
{
    pListCtrl->DeleteAllItems();

    auto date = event.GetDate();

    CalculateTotalTime(date);
    RefreshItems(date);

    pListCtrl->SetFocus();
}

void MainFrame::OnNewStopwatchTaskFromPausedStopwatchTask(wxCommandEvent& event)
{
    pTaskStorage->Store(pTaskState);
    pTaskState->mTimes.clear();

    dialog::StopwatchTaskDialog stopwatchTask(this, pConfig, pLogger, pTaskState, /* hasPendingPausedTask */ true);
    stopwatchTask.Launch();

    pTaskState->mTimes.clear();
    pTaskStorage->Restore(pTaskState);

    dialog::StopwatchTaskDialog stopwatchPausedTask(this, pConfig, pLogger, pTaskState);
    stopwatchPausedTask.Relaunch();

    pTaskStorage->mTimes.clear();
}

void MainFrame::OnCheckForUpdate(wxCommandEvent& event)
{
    dialog::CheckForUpdateDialog checkForUpdate(this);
    checkForUpdate.LaunchModal();
}

void MainFrame::OnResize(wxSizeEvent& event)
{
    auto frameSize = GetClientSize();

    if (bHasInitialized) {
        int width = frameSize.GetWidth();

        pListCtrl->SetColumnWidth(0, width * 0.10); // project
        pListCtrl->SetColumnWidth(1, width * 0.11); // task date
        pListCtrl->SetColumnWidth(2, width * 0.09); // start time
        pListCtrl->SetColumnWidth(3, width * 0.09); // end time
        pListCtrl->SetColumnWidth(4, width * 0.10); // duration
        pListCtrl->SetColumnWidth(5, width * 0.12); // category
        pListCtrl->SetColumnWidth(6, width * 0.37); // description
    }

    if (!bHasInitialized) {
        bHasInitialized = true;
    }

    event.Skip();
}

void MainFrame::CalculateTotalTime(wxDateTime date)
{
    std::vector<std::string> taskDurations;
    services::db_service dbService;

    auto dateString = date.FormatISODate();

    try {
        taskDurations = dbService.get_task_hours_by_id(dateString);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on get_task_hours_by_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    wxTimeSpan totalDuration;
    for (auto duration : taskDurations) {
        std::vector<std::string> durationSplit = util::lib::split(duration, ':');

        wxTimeSpan currentDuration(std::atol(durationSplit[0].c_str()),
            std::atol(durationSplit[1].c_str()),
            (wxLongLong) std::atoll(durationSplit[2].c_str()));

        totalDuration += currentDuration;
    }

    pTotalHoursText->SetLabel(totalDuration.Format(constants::TotalHours));
}

void MainFrame::RefreshItems(wxDateTime date)
{
    wxString dateString = date.FormatISODate();
    std::vector<models::task_item> taskItems;
    try {
        services::db_service dbService;
        taskItems = dbService.get_all_task_items_by_date(std::string(dateString.ToUTF8()));
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on get_all_task_items_by_date() - {0:d} : {1}", e.get_code(), e.what());
        return;
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto taskItem : taskItems) {
        listIndex = pListCtrl->InsertItem(columnIndex++, taskItem.project_name);
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem.task_date);
        pListCtrl->SetItem(
            listIndex, columnIndex++, taskItem.start_time ? wxString(*taskItem.start_time) : wxGetEmptyString());
        pListCtrl->SetItem(
            listIndex, columnIndex++, taskItem.end_time ? wxString(*taskItem.end_time) : wxGetEmptyString());
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem.duration);
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem.category_name);
        pListCtrl->SetItem(listIndex, columnIndex++, taskItem.description);

        pListCtrl->SetItemBackgroundColour(listIndex, wxColour(taskItem.category_color));
        unsigned int data = 0;
        data |= taskItem.task_item_id;
        data |= (taskItem.task_item_type_id << 8);
        pListCtrl->SetItemPtrData(listIndex, (wxUIntPtr) data);

        columnIndex = 0;
        taskItem.cleanup();
    }
}
} // namespace app::frame
