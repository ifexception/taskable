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

#pragma once

#include <memory>

#include <sqlite_modern_cpp.h>
#include <wx/wx.h>
#include <wx/taskbar.h>

#include <spdlog/spdlog.h>

#include "../config/configuration.h"

namespace app::frm
{
class TaskBarIcon : public wxTaskBarIcon
{
public:
    TaskBarIcon(wxFrame* parent,
        std::shared_ptr<cfg::Configuration> config,
        std::shared_ptr<spdlog::logger> logger);
    virtual ~TaskBarIcon() = default;

    void SetTaskBarIcon();

private:
    wxDECLARE_EVENT_TABLE();

    wxMenu* CreatePopupMenu() override;

    void OnNewEntryTask(wxCommandEvent& event);
    void OnNewTimedTask(wxCommandEvent& event);
    void OnPreferences(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnLeftButtonDown(wxTaskBarIconEvent& event);

    wxFrame* pParent;
    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<cfg::Configuration> pConfig;

    enum { ID_ADD_ENTRY_TASK = wxID_HIGHEST + 1, ID_ADD_TIMED_TASK, ID_SETTINGS };
};
} // namespace app::frm
