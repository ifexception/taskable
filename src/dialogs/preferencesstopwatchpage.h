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

#include <wx/wx.h>

namespace app::cfg
{
class Configuration;
}

namespace app::dlg
{
class StopwatchPage final : public wxPanel
{
public:
    StopwatchPage() = delete;
    StopwatchPage(wxWindow* parent, cfg::Configuration* config);
    virtual ~StopwatchPage() = default;

    void Apply();

private:
    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void OnMinimizeTimedTaskWindowCheck(wxCommandEvent& event);

    cfg::Configuration* pConfig;

    wxWindow* pParent;

    wxCheckBox* pMinimizeStopwatchWindowCtrl;
    wxChoice* pHideWindowTimeChoiceCtrl;
    wxChoice* pNotificationTimeChoiceCtrl;
    wxChoice* pPausedTaskReminderChoiceCtrl;
    wxCheckBox* pStartStopwatchOnLaunchCtrl;

    enum {
        IDC_MINIMIZE_STOPWATCH_WINDOW = wxID_HIGHEST + 1,
        IDC_HIDE_WINDOW_TIME_CHOICE,
        IDC_NOTIFICATION_TIME_CHOICE,
        IDC_PAUSED_TASK_REMINDER_CHOICE,
        IDC_START_STOPWATCH_ON_LAUNCH,
    };
};
}
