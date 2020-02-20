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

#include "taskbaricon.h"

#include <wx/taskbarbutton.h>

#include "../common/common.h"
#include "../dialogs/taskitemdlg.h"
#include "../dialogs/preferencesdlg.h"

namespace app::frm
{
// clang-format off
wxBEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(TaskBarIcon::ID_ADD_ENTRY_TASK, TaskBarIcon::OnNewEntryTask)
    EVT_MENU(TaskBarIcon::ID_ADD_TIMED_TASK, TaskBarIcon::OnNewTimedTask)
    EVT_MENU(TaskBarIcon::ID_SETTINGS, TaskBarIcon::OnPreferences)
    EVT_MENU(wxID_EXIT, TaskBarIcon::OnExit)
    EVT_TASKBAR_LEFT_DOWN(TaskBarIcon::OnLeftButtonDown)
wxEND_EVENT_TABLE()

TaskBarIcon::TaskBarIcon(wxFrame* parent,
    std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    sqlite::database* database)
    : pParent(parent)
    , pLogger(logger)
    , pConfig(config)
    , pDatabase(database)
// clang-format on
{
}

void TaskBarIcon::SetTaskBarIcon()
{
    SetIcon(wxIcon(common::GetProgramIcon()), common::GetProgramName());
}

wxMenu* TaskBarIcon::CreatePopupMenu()
{
    auto menu = new wxMenu();

    auto entryMenu = menu->Append(ID_ADD_ENTRY_TASK, wxT("Add Entry Task"));
    entryMenu->SetBitmap(common::GetEntryTaskIcon());

    auto timedMenuItem = menu->Append(ID_ADD_TIMED_TASK, wxT("Add Timed Task"));
    timedMenuItem->SetBitmap(common::GetTimedTaskIcon());

    menu->AppendSeparator();
    auto preferencesMenuItem = menu->Append(ID_SETTINGS, wxT("Preferences"));
    preferencesMenuItem->SetBitmap(common::GetSettingsIcon());

    menu->AppendSeparator();
    auto exitMenuItem = menu->Append(wxID_EXIT, wxT("Exit"));
    exitMenuItem->SetBitmap(common::GetQuitIcon());

    return menu;
}

void TaskBarIcon::OnNewEntryTask(wxCommandEvent& WXUNUSED(event))
{
    dlg::TaskItemDialog entryTask(pParent, pLogger, pConfig, constants::TaskItemTypes::EntryTask);
    entryTask.ShowModal();
}

void TaskBarIcon::OnNewTimedTask(wxCommandEvent& WXUNUSED(event))
{
    dlg::TaskItemDialog timedTask(pParent, pLogger, pConfig, constants::TaskItemTypes::TimedTask);
    timedTask.ShowModal();
}

void TaskBarIcon::OnPreferences(wxCommandEvent& WXUNUSED(event))
{
    dlg::PreferencesDialog preferences(pParent, pConfig, pLogger, this);
    preferences.ShowModal();
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
} // namespace app::frm
