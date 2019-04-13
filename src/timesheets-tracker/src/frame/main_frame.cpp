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

#include "main_frame.h"

#include "../common/ids.h"
#include "../dialogs/about_dialog.h"
#include "../dialogs/task_details_dialog.h"
#include "../dialogs/employer_dialog.h"
#include "../dialogs/client_dialog.h"
#include "../dialogs/project_dialog.h"
#include "../dialogs/category_dialog.h"
#include "../services/db_service.h"

namespace app::frame
{
const int TaskId = static_cast<int>(ids::MenuIds::TaskDetailMenuId);
const int EmployerId = static_cast<int>(ids::MenuIds::EmployerMenuId);
const int ProjectId = static_cast<int>(ids::MenuIds::ProjectMenuId);
const int ClientId = static_cast<int>(ids::MenuIds::ClientMenuId);
const int CategoryId = static_cast<int>(ids::MenuIds::CategoryMenuId);

wxBEGIN_EVENT_TABLE(main_frame, wxFrame)
    EVT_MENU(wxID_ABOUT, main_frame::on_about)
    EVT_MENU(TaskId, main_frame::on_new_task)
    EVT_MENU(EmployerId, main_frame::on_new_employer)
    EVT_MENU(ProjectId, main_frame::on_new_project)
    EVT_MENU(ClientId, main_frame::on_new_client)
    EVT_MENU(CategoryId, main_frame::on_new_category)
wxEND_EVENT_TABLE()

main_frame::main_frame(wxWindow* parent, const wxString& name)
    : wxFrame(parent, wxID_ANY, "Tasks Tracker", wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE, name)
{
    bool success = create(/*parent, wxID_ANY, wxT("Tasks Tracker"), wxDefaultPosition, wxSize(640, 480), wxDEFAULT_FRAME_STYLE, name*/);
    SetMinClientSize(wxSize(640, 480));
}

main_frame::~main_frame()
{
    wxLogDebug(wxT("Destroy FRAME"));
}

bool main_frame::create(/*wxWindow * parent, wxWindowID windowId, const wxString& title, const wxPoint& point, const wxSize& size, long style, const wxString& name*/)
{
    //bool created = new wxFrame(parent, windowId, title, point, size, style, name);
    //if (created) {
    create_controls();
    data_to_controls();
    //}
    //return created;
    return true;
}

void main_frame::create_controls()
{
    /* File Menu Control */
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(TaskId, "New &Task...\tCtrl-N", "Create new task");
    fileMenu->Append(EmployerId, "New &Employer...\tCtrl-E", "Create new employer");
    fileMenu->Append(ClientId, "New &Client...\tCtrl-L", "Create new client");
    fileMenu->Append(ProjectId, "New &Project...\tCtrl-R", "Create new project");
    fileMenu->Append(CategoryId, wxT("New C&ategory...\tCtrl-A"), wxT("Create new category"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    /* Edit Menu Control */
    auto editMenu = new wxMenu();

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

    pListCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);
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
    pListCtrl->InsertColumn(4, durationColumn);

    wxListItem categoryColumn;
    categoryColumn.SetId(5);
    categoryColumn.SetText(wxT("Category"));
    categoryColumn.SetWidth(96);
    pListCtrl->InsertColumn(5, categoryColumn);

    wxListItem descriptionColumn;
    descriptionColumn.SetId(6);
    descriptionColumn.SetText(wxT("Description"));
    descriptionColumn.SetWidth(205);
    pListCtrl->InsertColumn(6, descriptionColumn);
}

void main_frame::data_to_controls()
{
    std::vector<models::detailed_task> detailedTasks;
    try {
        services::db_service dbService;
        detailedTasks = dbService.get_all_tasks_by_date("2019-04-10");
    } catch (const std::exception&) {

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
        columnIndex = 0;
    }
}

void main_frame::on_about(wxCommandEvent& event)
{
    dialog::about_dialog about(nullptr);
}

void main_frame::on_close(wxCommandEvent& event)
{
    Close(true);
}

void main_frame::on_new_task(wxCommandEvent& event)
{
    dialog::task_details_dialog newTask(this);
    newTask.launch_task_details_dialog();
}

void main_frame::on_new_employer(wxCommandEvent& event)
{
    dialog::employer_dialog newEmployer(this);
    newEmployer.launch_employer_dialog();
}

void main_frame::on_new_client(wxCommandEvent& event)
{
    dialog::client_dialog newClient(this);
    newClient.launch_client_dialog();
}

void main_frame::on_new_project(wxCommandEvent& event)
{
    dialog::project_dialog newProject(this);
    newProject.launch_project_dialog();
}

void main_frame::on_new_category(wxCommandEvent& event)
{
    dialog::category_dialog categoryDialog(this);
    categoryDialog.launch_dialog();
}
} // namespace app::frame
