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

#include "main_frame.hh"

#include "../common/ids.hh"
#include "../dialogs/about_dialog.hh"
#include "../dialogs/task_details_dialog.hh"
#include "../dialogs/employer_dialog.hh"
#include "../dialogs/project_dialog.hh"

namespace app::frame
{
main_frame::main_frame() : wxFrame(nullptr, wxID_ANY, "Timesheets Tracker")
{
    const int TaskId = static_cast<int>(ids::MenuIds::TaskDetailMenuId);
    const int EmployerId = static_cast<int>(ids::MenuIds::EmployerMenuId);
    const int ProjectId = static_cast<int>(ids::MenuIds::ProjectMenuId);

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(TaskId, "&New Task...\tCtrl-N", "Create new timesheet entry");
    fileMenu->Append(EmployerId, "&New Employer...\tCtrl-E", "Create new employer");
    fileMenu->Append(ProjectId, "&New Project...\tCtrl-R", "Create new project");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    auto editMenu = new wxMenu();

    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) { dialog::about_dialog about(nullptr); }, wxID_ABOUT);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    Bind(wxEVT_MENU,
         [=](wxCommandEvent&) {
             dialog::task_details_dialog newTask(nullptr);
             newTask.launch_task_details_dialog();
         },
         TaskId);

    Bind(wxEVT_MENU,
         [=](wxCommandEvent&) {
             dialog::employer_dialog newEmployer(nullptr);
             newEmployer.launch_employer_dialog();
         },
         EmployerId);

    Bind(wxEVT_MENU,
        [=](wxCommandEvent&) {
            dialog::project_dialog newProject(nullptr);
            newProject.launch_project_dialog();
        },
        ProjectId);
}

main_frame::~main_frame()
{
    Destroy();
}
} // namespace app::frame
