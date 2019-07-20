//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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

#include <wx/aboutdlg.h>
#include <wx/taskbarbutton.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../common/version.h"
#include "../dialogs/taskitemdialog.h"
#include "../dialogs/employerdialog.h"
#include "../dialogs/clientdialog.h"
#include "../dialogs/projectdialog.h"
#include "../dialogs/categorydialog.h"
#include "../dialogs/editlistdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/timedtaskdialog.h"
#include "../services/db_service.h"
#include "../db/database_exception.h"
#include "../wizards/setupwizard.h"
#include "taskbaricon.h"

namespace app::frame
{
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(ids::ID_NEW_TASK, MainFrame::OnNewTask)
EVT_MENU(ids::ID_NEW_EMPLOYER, MainFrame::OnNewEmployer)
EVT_MENU(ids::ID_NEW_PROJECT, MainFrame::OnNewProject)
EVT_MENU(ids::ID_NEW_CLIENT, MainFrame::OnNewClient)
EVT_MENU(ids::ID_NEW_CATEGORY, MainFrame::OnNewCategory)
EVT_MENU(ids::ID_EDIT_EMPLOYER, MainFrame::OnEditEmployer)
EVT_MENU(ids::ID_EDIT_CLIENT, MainFrame::OnEditClient)
EVT_MENU(ids::ID_EDIT_PROJECT, MainFrame::OnEditProject)
EVT_MENU(ids::ID_EDIT_CATEGORY, MainFrame::OnEditCategory)
EVT_MENU(ids::ID_SETTINGS, MainFrame::OnSettings)
EVT_MENU(ids::ID_NEW_TIMED_TASK, MainFrame::OnNewTimedTask)
EVT_LIST_ITEM_ACTIVATED(MainFrame::IDC_LIST, MainFrame::OnItemDoubleClick)
EVT_COMMAND(MainFrame::IDC_LIST, ids::ID_TASK_INSERTED, MainFrame::OnTaskInserted)
EVT_ICONIZE(MainFrame::OnIconize)
EVT_DATE_CHANGED(MainFrame::IDC_GO_TO_DATE, MainFrame::OnDateChanged)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<cfg::Configuration> config, const wxString& name)
    : wxFrame(nullptr, wxID_ANY, wxT("Tasks Tracker"), wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE, name)
    , pConfig(config)
{ }

MainFrame::~MainFrame()
{
    if (pTaskBarIcon != nullptr) {
        delete pTaskBarIcon;
    }
}

bool MainFrame::RunWizard()
{
    auto wizard = new wizard::SetupWizard(this);
    wizard->CenterOnParent();
    bool wizardSetupSuccess = wizard->Run();
    return wizardSetupSuccess;
}

bool MainFrame::CreateFrame()
{
    bool success = Create();
    SetMinClientSize(wxSize(640, 480));
    SetIcon(common::GetProgramIcon());
    pTaskBarIcon = new TaskBarIcon(this, pConfig);
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
    SetStatusText(wxString::Format("%d.%d.%d", TASKS_TRACKER_MAJOR, TASKS_TRACKER_MINOR, TASKS_TRACKER_PATCH), 1);

    /* File Menu Control */
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ids::ID_NEW_TASK, wxT("New &Task...\tCtrl-N"), wxT("Create new task"));
    fileMenu->Append(ids::ID_NEW_TIMED_TASK, wxT("&Timed Task...\tCtrl-Q"), wxT("Create new timed task"));
    fileMenu->Append(ids::ID_NEW_EMPLOYER, wxT("New &Employer"), wxT("Create new employer"));
    fileMenu->Append(ids::ID_NEW_CLIENT, wxT("New &Client"), wxT("Create new client"));
    fileMenu->Append(ids::ID_NEW_PROJECT, wxT("New &Project"), wxT("Create new project"));
    fileMenu->Append(ids::ID_NEW_CATEGORY, wxT("New C&ategory"), wxT("Create new category"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    /* Edit Menu Control */
    auto editMenu = new wxMenu();
    editMenu->Append(ids::ID_EDIT_EMPLOYER, wxT("Edit &Employers"), wxT("Select a employer to edit"));
    editMenu->Append(ids::ID_EDIT_CLIENT, wxT("Edit &Client"), wxT("Select a client to edit"));
    editMenu->Append(ids::ID_EDIT_PROJECT, wxT("Edit &Project"), wxT("Select a project to edit"));
    editMenu->Append(ids::ID_EDIT_CATEGORY, wxT("Edit C&ategory"), wxT("Select a category to edit"));
    editMenu->AppendSeparator();
    editMenu->Append(ids::ID_SETTINGS, wxT("&Settings...\tCtrl-P"), wxT("Edit application settings"));

    /* Export Menu Control */
    auto exportMenu = new wxMenu();

    /* Help Menu Control */
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT);

    /* Menu Bar */
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(exportMenu, wxT("E&xport"));
    menuBar->Append(helpMenu, wxT("&Help"));

    SetMenuBar(menuBar);

    /* Accelerator Table */
    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_CTRL, (int)'N', ids::ID_NEW_TASK);
    entries[1].Set(wxACCEL_CTRL, (int)'P', ids::ID_SETTINGS);
    entries[2].Set(wxACCEL_CTRL, (int)'H', wxID_ABOUT);
    entries[3].Set(wxACCEL_CTRL, (int)'Q', ids::ID_NEW_TIMED_TASK);

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
    const auto& flags = common::sizers::ControlCenterVertical;
    utilSizer->Add(gotoText, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    pDatePickerCtrl = new wxDatePickerCtrl(utilPanel, IDC_GO_TO_DATE, wxDefaultDateTime, wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    pDatePickerCtrl->SetToolTip(wxT("Select a date to navigate to"));
    utilSizer->Add(pDatePickerCtrl, common::sizers::ControlDefault);

    pTotalHoursText = new wxStaticText(utilPanel, IDC_HOURS_TEXT, wxT("Total Hours: %d"));
    pTotalHoursText->SetToolTip(wxT("Indicates the total hours spent on a given task for today"));
    utilSizer->AddStretchSpacer();
    utilSizer->Add(pTotalHoursText, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    /* List Panel and Control*/
    auto listPanel = new wxPanel(mainPanel);
    mainSizer->Add(listPanel, 1, wxEXPAND);
    auto listSizer = new wxBoxSizer(wxHORIZONTAL);
    listPanel->SetSizer(listSizer);

    pListCtrl = new wxListCtrl(listPanel, IDC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
    pListCtrl->SetFocus();
    listSizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);

    wxListItem projectColumn;
    projectColumn.SetId(0);
    projectColumn.SetText(wxT("Project"));
    projectColumn.SetWidth(90);
    pListCtrl->InsertColumn(0, projectColumn);

    wxListItem dateColumn;
    dateColumn.SetId(1);
    dateColumn.SetText(wxT("Date"));
    pListCtrl->InsertColumn(1, dateColumn);

    wxListItem startTimeColumn;
    startTimeColumn.SetId(2);
    startTimeColumn.SetText(wxT("Started"));
    startTimeColumn.SetWidth(56);
    pListCtrl->InsertColumn(2, startTimeColumn);

    wxListItem endTimeColumn;
    endTimeColumn.SetId(3);
    endTimeColumn.SetText(wxT("Ended"));
    endTimeColumn.SetWidth(56);
    pListCtrl->InsertColumn(3, endTimeColumn);

    wxListItem durationColumn;
    durationColumn.SetId(4);
    durationColumn.SetText(wxT("Duration"));
    durationColumn.SetWidth(60);
    pListCtrl->InsertColumn(4, durationColumn);

    wxListItem categoryColumn;
    categoryColumn.SetId(5);
    categoryColumn.SetText(wxT("Category"));
    categoryColumn.SetWidth(96);
    pListCtrl->InsertColumn(5, categoryColumn);

    wxListItem descriptionColumn;
    descriptionColumn.SetId(6);
    descriptionColumn.SetText(wxT("Description"));
    descriptionColumn.SetWidth(214);
    pListCtrl->InsertColumn(6, descriptionColumn);
}

void MainFrame::DataToControls()
{
    std::vector<std::string> taskDurations;
    services::db_service dbService;
    auto dateNow = wxDateTime::Now();
    auto dateString = dateNow.FormatISODate();
    try {
        taskDurations = dbService.get_task_hours_by_id(dateString);
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    wxTimeSpan totalDuration;
    for (auto duration : taskDurations) {
        std::vector<std::string> durationSplit = util::lib::split(duration, ':');
        wxTimeSpan currentDuration(std::atol(durationSplit[0].c_str()), std::atol(durationSplit[1].c_str()), (wxLongLong)std::atoll(durationSplit[2].c_str()));
        totalDuration += currentDuration;
    }

    pTotalHoursText->SetLabel(totalDuration.Format(Constants::TotalHours));

    RefreshItems();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetIcon(common::GetProgramIcon());
    aboutInfo.SetName(wxT("TasksTracker"));
    aboutInfo.SetVersion(wxString::Format("%d.%d.%d", TASKS_TRACKER_MAJOR, TASKS_TRACKER_MINOR, TASKS_TRACKER_PATCH));
    aboutInfo.SetDescription(wxT("A desktop application to help you manage how you've spent your time on tasks during the day\n by tracking the time you've spent on those tasks throughout the day"));
    aboutInfo.SetCopyright("(C) 2018-2019");
    aboutInfo.SetWebSite(wxT("https://github.com/ifexception/wx-tasks-tracker"));
    aboutInfo.SetLicence(common::GetLicense());
    aboutInfo.AddDeveloper(wxT("Szymon Welgus"));

    wxAboutBox(aboutInfo);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if (pConfig->IsConfirmOnExit() && event.CanVeto()) {
        int ret = wxMessageBox(wxT("Are you sure to exit the application?"), wxT("Tasks Tracker"), wxICON_QUESTION | wxYES_NO);
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

void MainFrame::OnNewTask(wxCommandEvent& event)
{
    dialog::TaskItemDialog newTask(this);
    newTask.ShowModal();
}

void MainFrame::OnNewEmployer(wxCommandEvent& event)
{
    dialog::EmployerDialog newEmployer(this);
    newEmployer.ShowModal();
}

void MainFrame::OnNewClient(wxCommandEvent& event)
{
    dialog::ClientDialog newClient(this);
    newClient.ShowModal();
}

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    dialog::ProjectDialog newProject(this);
    newProject.ShowModal();
}

void MainFrame::OnNewCategory(wxCommandEvent& event)
{
    dialog::CategoryDialog categoryDialog(this);
    categoryDialog.ShowModal();
}

void MainFrame::OnEditEmployer(wxCommandEvent& event)
{
    dialog::EditListDialog employerEdit(this, dialog::dialog_type::Employer);
    employerEdit.ShowModal();
}

void MainFrame::OnEditClient(wxCommandEvent& event)
{
    dialog::EditListDialog clientEdit(this, dialog::dialog_type::Client);
    clientEdit.ShowModal();
}

void MainFrame::OnEditProject(wxCommandEvent& event)
{
    dialog::EditListDialog projectEdit(this, dialog::dialog_type::Project);
    projectEdit.ShowModal();
}

void MainFrame::OnEditCategory(wxCommandEvent& event)
{
    dialog::EditListDialog categoryEdit(this, dialog::dialog_type::Category);
    categoryEdit.ShowModal();
}

void MainFrame::OnTaskInserted(wxCommandEvent& event)
{
    pListCtrl->DeleteAllItems();

    RefreshItems();
}

void MainFrame::OnItemDoubleClick(wxListEvent& event)
{
    int taskDetailId = event.GetData();
    dialog::TaskItemDialog editTask(this, true, taskDetailId);
    editTask.ShowModal();
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

void MainFrame::OnNewTimedTask(wxCommandEvent& event)
{
    dialog::TimedTaskDialog timedTask(this, pConfig);
    timedTask.Launch();
}

void MainFrame::OnDateChanged(wxDateEvent& event)
{
    auto date = event.GetDate();
    RefreshItems(date);
}

void MainFrame::RefreshItems(wxDateTime date)
{
    wxString dateString = date.FormatISODate();
    std::vector<models::detailed_task> detailedTasks;
    try {
        services::db_service dbService;
        detailedTasks = dbService.get_all_task_items_by_date(std::string(dateString.ToUTF8()));
    } catch (const db::database_exception& e) {
        wxLogError(wxString::Format("Error %d", e.get_error_code()));
    }

    int listIndex = 0;
    int columnIndex = 0;
    for (auto task : detailedTasks) {
        listIndex = pListCtrl->InsertItem(columnIndex++, task.project_name);
        pListCtrl->SetItem(listIndex, columnIndex++, task.task_date);
        pListCtrl->SetItem(listIndex, columnIndex++, task.start_time);
        pListCtrl->SetItem(listIndex, columnIndex++, task.end_time);
        pListCtrl->SetItem(listIndex, columnIndex++, task.duration);
        pListCtrl->SetItem(listIndex, columnIndex++, task.category_name);
        pListCtrl->SetItem(listIndex, columnIndex++, task.description);
        pListCtrl->SetItemPtrData(listIndex, task.task_detail_id);
        columnIndex = 0;
    }
}
} // namespace app::frame
