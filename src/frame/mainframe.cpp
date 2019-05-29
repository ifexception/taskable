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

#include "../common/ids.h"
#include "../dialogs/aboutdialog.h"
#include "../dialogs/taskdetailsdialog.h"
#include "../dialogs/employerdialog.h"
#include "../dialogs/clientdialog.h"
#include "../dialogs/projectdialog.h"
#include "../dialogs/categorydialog.h"
#include "../dialogs/editlistdialog.h"
#include "../services/db_service.h"
#include "../db/database_exception.h"
#include "../wizards/setupwizard.h"

namespace app::frame
{
const int TaskId = static_cast<int>(ids::MenuIds::TaskDetailMenuId);
const int EmployerId = static_cast<int>(ids::MenuIds::EmployerMenuId);
const int ProjectId = static_cast<int>(ids::MenuIds::ProjectMenuId);
const int ClientId = static_cast<int>(ids::MenuIds::ClientMenuId);
const int CategoryId = static_cast<int>(ids::MenuIds::CategoryMenuId);

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(TaskId, MainFrame::OnNewTask)
    EVT_MENU(EmployerId, MainFrame::OnNewEmployer)
    EVT_MENU(ProjectId, MainFrame::OnNewProject)
    EVT_MENU(ClientId, MainFrame::OnNewClient)
    EVT_MENU(CategoryId, MainFrame::OnNewCategory)
    EVT_MENU(ids::ID_EDIT_EMPLOYER, MainFrame::OnEditEmployer)
    EVT_MENU(ids::ID_EDIT_CLIENT, MainFrame::OnEditClient)
    EVT_MENU(ids::ID_EDIT_PROJECT, MainFrame::OnEditProject)
    EVT_MENU(ids::ID_EDIT_CATEGORY, MainFrame::OnEditCategory)
    EVT_LIST_ITEM_ACTIVATED(MainFrame::IDC_LIST, MainFrame::OnItemDoubleClick)
    EVT_COMMAND(MainFrame::IDC_LIST, ids::ID_TASK_INSERTED, MainFrame::OnTaskInserted)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent, const wxString& name)
    : wxFrame(parent, wxID_ANY, wxT("Tasks Tracker"), wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE, name)
{
    bool success = Create(/*parent, wxID_ANY, wxT("Tasks Tracker"), wxDefaultPosition, wxSize(640, 480), wxDEFAULT_FRAME_STYLE, name*/);
    SetMinClientSize(wxSize(640, 480));
}

MainFrame::~MainFrame()
{}

bool MainFrame::OnStartUp()
{
    auto wizard = new wizard::SetupWizard(this);
    wizard->CenterOnParent();
    bool wizardSetupSuccess = wizard->Run();
    return wizardSetupSuccess;
}

bool MainFrame::Create(/*wxWindow * parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name*/)
{
    //bool created = new wxFrame(parent, windowId, title, point, size, style, name);
    //if (created) {
    CreateControls();
    DataToControls();
    //}
    //return created;
    return true;
}

void MainFrame::CreateControls()
{
    /* Status Bar Control */
    CreateStatusBar(1);
    SetStatusText(wxT("Ready"));

    /* File Menu Control */
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(TaskId, wxT("New &Task...\tCtrl-N"), wxT("Create new task"));
    fileMenu->Append(EmployerId, wxT("New &Employer...\tCtrl-E"), wxT("Create new employer"));
    fileMenu->Append(ClientId, wxT("New &Client...\tCtrl-L"), wxT("Create new client"));
    fileMenu->Append(ProjectId, wxT("New &Project...\tCtrl-R"), wxT("Create new project"));
    fileMenu->Append(CategoryId, wxT("New C&ategory...\tCtrl-A"), wxT("Create new category"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    /* Edit Menu Control */
    auto editMenu = new wxMenu();
    editMenu->Append(ids::ID_EDIT_EMPLOYER, wxT("Edit &Employers"), wxT("Select a employer to edit"));
    editMenu->Append(ids::ID_EDIT_CLIENT, wxT("Edit &Client"), wxT("Select a client to edit"));
    editMenu->Append(ids::ID_EDIT_PROJECT, wxT("Edit &Project"), wxT("Select a project to edit"));
    editMenu->Append(ids::ID_EDIT_CATEGORY, wxT("Edit C&ategory"), wxT("Select a category to edit"));

    /* Help Menu Control */
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT);

    /* Menu Bar */
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    auto panel = new wxPanel(this);
    auto sizer = new wxBoxSizer(wxHORIZONTAL);

    pListCtrl = new wxListCtrl(panel, IDC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
    sizer->Add(pListCtrl, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);

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
    RefreshItems();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    dialog::AboutDialog about(nullptr);
}

void MainFrame::OnClose(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnNewTask(wxCommandEvent& event)
{
    dialog::TaskDetailsDialog newTask(this);
    newTask.Launch();
}

void MainFrame::OnNewEmployer(wxCommandEvent& event)
{
    dialog::EmployerDialog newEmployer(this);
    newEmployer.Launch();
}

void MainFrame::OnNewClient(wxCommandEvent& event)
{
    dialog::ClientDialog newClient(this);
    newClient.Launch();
}

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    dialog::ProjectDialog newProject(this);
    newProject.Launch();
}

void MainFrame::OnNewCategory(wxCommandEvent& event)
{
    dialog::CategoryDialog categoryDialog(this);
    categoryDialog.Launch();
}

void MainFrame::OnEditEmployer(wxCommandEvent& event)
{
    dialog::EditListDialog employerEdit(this, dialog::dialog_type::Employer);
    employerEdit.Launch();
}

void MainFrame::OnEditClient(wxCommandEvent& event)
{
    dialog::EditListDialog clientEdit(this, dialog::dialog_type::Client);
    clientEdit.Launch();
}

void MainFrame::OnEditProject(wxCommandEvent& event)
{
    dialog::EditListDialog projectEdit(this, dialog::dialog_type::Project);
    projectEdit.Launch();
}

void MainFrame::OnEditCategory(wxCommandEvent& event)
{
    dialog::EditListDialog categoryEdit(this, dialog::dialog_type::Category);
    categoryEdit.Launch();
}

void MainFrame::OnTaskInserted(wxCommandEvent& event)
{
    pListCtrl->DeleteAllItems();

    RefreshItems();
}

void MainFrame::OnItemDoubleClick(wxListEvent& event)
{
    int taskDetailId = event.GetData();
    dialog::TaskDetailsDialog editTask(this, true, taskDetailId);
    editTask.Launch();
}

void MainFrame::RefreshItems()
{
    wxDateTime date = wxDateTime::Now();
    wxString dateString = date.FormatISODate();
    std::vector<models::detailed_task> detailedTasks;
    try {
        services::db_service dbService;
        detailedTasks = dbService.get_all_tasks_by_date(std::string(dateString.ToUTF8())); //FIXME
    }
    catch (const db::database_exception& e) {
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
