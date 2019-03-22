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

#include "../common/menu_ids.hh"
#include "../dialogs/task_details_dialog.hh"
#include "../dialogs/about_dialog.hh"

namespace app::frame
{
main_frame::main_frame() : wxFrame(nullptr, wxID_ANY, "Timesheets Tracker")
{
    const int newTaskId = static_cast<int>(MenuIds::NewTaskId);

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(newTaskId, "&New Task...\tCtrl-N", "Create new timesheet entry");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) { dialog::about_dialog about(nullptr); }, wxID_ABOUT);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    Bind(wxEVT_MENU,
         [=](wxCommandEvent&) {
             dialog::task_details_dialog newTask(nullptr);
             newTask.create_task_details();
         },
         newTaskId);
}

main_frame::~main_frame()
{
}
} // namespace app::frame
