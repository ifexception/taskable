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

#include "taskbaricon.h"

#include <wx/taskbarbutton.h>

#include "../common/common.h"
#include "../dialogs/taskdetailsdialog.h"

namespace app::frame
{
wxBEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(TaskBarIcon::ID_ADD_TASK, TaskBarIcon::OnNewTask)
    EVT_MENU(TaskBarIcon::ID_SETTINGS, TaskBarIcon::OnSettings)
    EVT_MENU(wxID_EXIT, TaskBarIcon::OnExit)
    EVT_TASKBAR_LEFT_DOWN(TaskBarIcon::OnLeftButtonDown)
wxEND_EVENT_TABLE()

TaskBarIcon::TaskBarIcon(wxFrame* parent)
    : pParent(parent)
{ }

void TaskBarIcon::SetTaskBarIcon()
{
    SetIcon(wxIcon(common::GetProgramIcon()), wxT("Tasks Tracker"));
}

//bool TaskBarIcon::PopupMenu(wxMenu * menu)
//{
//    return false;
//}

wxMenu* TaskBarIcon::CreatePopupMenu()
{
    auto menu = new wxMenu();

    menu->Append(ID_ADD_TASK, wxT("Add Task"));
    menu->AppendSeparator();
    menu->Append(ID_SETTINGS, wxT("Settings"));
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, wxT("Exit"));

    return menu;
}

void TaskBarIcon::OnNewTask(wxCommandEvent& WXUNUSED(event))
{
    dialog::TaskDetailsDialog newTask(pParent);
    newTask.Launch();
}

void TaskBarIcon::OnSettings(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("This feature is not yet available"), wxT("Tasks Tracker"), wxICON_INFORMATION | wxOK_DEFAULT);
}

void TaskBarIcon::OnExit(wxCommandEvent& WXUNUSED(event))
{
    pParent->Close(true);
}

void TaskBarIcon::OnLeftButtonDown(wxTaskBarIconEvent& WXUNUSED(event))
{
    pParent->MSWGetTaskBarButton()->Show();
    pParent->Restore();
    pParent->Raise();
    pParent->Show();
}

}
