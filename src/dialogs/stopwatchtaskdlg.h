//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
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

#pragma once

#include <memory>

#include <wx/wx.h>

#include <spdlog/spdlog.h>

#include "../config/configuration.h"
#include "../services/taskstateservice.h"
#include "../frame/taskbaricon.h"

wxDECLARE_EVENT(START_NEW_STOPWATCH_TASK, wxCommandEvent);

namespace app::dlg
{
class StopwatchTaskDialog : public wxDialog
{
public:
    StopwatchTaskDialog() = delete;

    explicit StopwatchTaskDialog(wxWindow* parent,
        std::shared_ptr<cfg::Configuration> config,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<services::TaskStateService> taskState,
        frm::TaskBarIcon *taskBarIcon,
        const wxString& name = wxT("stopwatchtaskdlg"));

    explicit StopwatchTaskDialog(wxWindow* parent,
        std::shared_ptr<cfg::Configuration> config,
        std::shared_ptr<spdlog::logger> logger,
        std::shared_ptr<services::TaskStateService> taskState,
        frm::TaskBarIcon *taskBarIcon,
        bool hasPendingPausedTask,
        const wxString& name = wxT("stopwatchtaskdlg"));

    virtual ~StopwatchTaskDialog() = default;

    void Launch();
    void Relaunch();

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();

    void ExecuteStartupProcedure();
    void ExecutePauseProcedure();
    void ExecuteStopProcedure();

    void OnElapsedTimeUpdate(wxTimerEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnHideWindow(wxTimerEvent& event);
    void OnPausedTaskReminder(wxTimerEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<cfg::Configuration> pConfig;
    std::shared_ptr<services::TaskStateService> pTaskState;

    wxWindow* pParent;
    wxStaticText* pElapsedTimeText;
    wxStaticText* pAccumulatedTimeText;
    wxCheckBox* pStartNewTask;
    wxTextCtrl* pStopwatchDescription;
    wxButton* pStartButton;
    wxButton* pPauseButton;
    wxButton* pStopButton;
    wxButton* pCancelButton;
    std::unique_ptr<wxTimer> pElapsedTimer;
    std::unique_ptr<wxTimer> pNotificationTimer;
    std::unique_ptr<wxTimer> pHideWindowTimer;
    std::unique_ptr<wxTimer> pPausedTaskReminder;
    frm::TaskBarIcon* pTaskBarIcon;

    wxDateTime mStartTime;
    wxDateTime mEndTime;
    bool bIsPaused;
    bool bWasTaskPaused;
    bool bHasPendingPausedTask;

    enum {
        IDC_ELAPSED = wxID_HIGHEST + 1,
        IDC_ACCUMULATED_TIME,
        IDC_START_NEW_TASK_CHECK,
        IDC_TASK_DESCRIPTION,
        IDC_ELAPSED_TIMER,
        IDC_NOTIFICATION_TIMER,
        IDC_HIDE_WINDOW_TIMER,
        IDC_PAUSED_TASK_REMINDER,
        IDC_START,
        IDC_PAUSE,
        IDC_STOP,
        IDC_CANCEL
    };
};
} // namespace app::dlg
