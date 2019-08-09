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
#include "../dialogs/taskitemdialog.h"
#include "../dialogs/settingsdialog.h"

namespace app::frame
{
// clang-format off
wxBEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(TaskBarIcon::ID_ADD_TASK, TaskBarIcon::OnNewTask)
    EVT_MENU(TaskBarIcon::ID_SETTINGS, TaskBarIcon::OnSettings)
    EVT_MENU(wxID_EXIT, TaskBarIcon::OnExit)
    EVT_TASKBAR_LEFT_DOWN(TaskBarIcon::OnLeftButtonDown)
wxEND_EVENT_TABLE()
// clang-format on

TaskBarIcon::TaskBarIcon(wxFrame* parent,
    std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger)
    : pParent(parent)
    , pLogger(logger)
    , pConfig(config)
{
}

void TaskBarIcon::SetTaskBarIcon()
{
    SetIcon(wxIcon(common::GetProgramIcon()), wxT("Tasks Tracker"));
}

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
    dialog::TaskItemDialog newTask(pParent, pLogger);
    newTask.ShowModal();
}

void TaskBarIcon::OnSettings(wxCommandEvent& WXUNUSED(event))
{
    dialog::SettingsDialog settings(pParent, pConfig);
    settings.ShowModal();
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

} // namespace app::frame
