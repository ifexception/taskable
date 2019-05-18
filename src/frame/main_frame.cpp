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
#include "../dialogs/edit_list_dialog.h"
#include "../services/db_service.h"

namespace app::frame
{
const int TaskId = static_cast<int>(ids::MenuIds::TaskDetailMenuId);
const int EmployerId = static_cast<int>(ids::MenuIds::EmployerMenuId);
const int ProjectId = static_cast<int>(ids::MenuIds::ProjectMenuId);
const int ClientId = static_cast<int>(ids::MenuIds::ClientMenuId);
const int CategoryId = static_cast<int>(ids::MenuIds::CategoryMenuId);
const int EmployerEditId = static_cast<int>(ids::MenuIds::EmployerEditId);

wxBEGIN_EVENT_TABLE(main_frame, wxFrame)
    EVT_MENU(wxID_ABOUT, main_frame::on_about)
    EVT_MENU(TaskId, main_frame::on_new_task)
    EVT_MENU(EmployerId, main_frame::on_new_employer)
    EVT_MENU(ProjectId, main_frame::on_new_project)
    EVT_MENU(ClientId, main_frame::on_new_client)
    EVT_MENU(CategoryId, main_frame::on_new_category)
    EVT_MENU(EmployerEditId, main_frame::on_edit_employer)
    EVT_LIST_ITEM_ACTIVATED(main_frame::IDC_LIST, main_frame::on_item_double_click)
    EVT_COMMAND(main_frame::IDC_LIST, ids::ID_TASK_INSERTED, main_frame::on_task_inserted)
wxEND_EVENT_TABLE()

main_frame::main_frame(wxWindow* parent, const wxString& name)
    : wxFrame(parent, wxID_ANY, wxT("Tasks Tracker"), wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE, name)
{
    bool success = create(/*parent, wxID_ANY, wxT("Tasks Tracker"), wxDefaultPosition, wxSize(640, 480), wxDEFAULT_FRAME_STYLE, name*/);
    SetMinClientSize(wxSize(640, 480));
}

main_frame::~main_frame()
{}

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
    editMenu->Append(EmployerEditId, wxT("Edit &Employers"), wxT("Select a employer to edit"));

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

void main_frame::data_to_controls()
{
    refresh_items();
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

void main_frame::on_edit_employer(wxCommandEvent& event)
{
    dialog::edit_list_dialog employerEdit(this, dialog::dialog_type::Employer);
    employerEdit.launch_dialog();
}

void main_frame::on_edit_client(wxCommandEvent & event)
{
    dialog::edit_list_dialog clientEdit(this, dialog::dialog_type::Client);
    clientEdit.launch_dialog();
}

void main_frame::on_edit_project(wxCommandEvent & event)
{
    dialog::edit_list_dialog projectEdit(this, dialog::dialog_type::Project);
    projectEdit.launch_dialog();
}

void main_frame::on_edit_category(wxCommandEvent & event)
{
    dialog::edit_list_dialog categoryEdit(this, dialog::dialog_type::Category);
    categoryEdit.launch_dialog();
}

void main_frame::on_task_inserted(wxCommandEvent& event)
{
    pListCtrl->DeleteAllItems();

    refresh_items();
}

void main_frame::on_item_double_click(wxListEvent& event)
{
    int taskDetailId = event.GetData();
    dialog::task_details_dialog editTask(this, true, taskDetailId);
    editTask.launch_task_details_dialog();
}

void main_frame::refresh_items()
{
    wxDateTime date = wxDateTime::Now();
    wxString dateString = date.FormatISODate();
    std::vector<models::detailed_task> detailedTasks;
    try {
        services::db_service dbService;
        detailedTasks = dbService.get_all_tasks_by_date(std::string(dateString.ToUTF8())); //FIXME
    }
    catch (const std::exception&) {

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
