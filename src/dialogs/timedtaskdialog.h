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

#pragma once

#include <memory>

#include <wx/wx.h>
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "../config/configuration.h"
#include "../services/taskstateservice.h"

wxDECLARE_EVENT(START_NEW_TIMED_TASK, wxCommandEvent);

namespace app::dialog
{
class TimedTaskDialog : public wxDialog
{
public:
    TimedTaskDialog() = delete;
    explicit TimedTaskDialog(wxWindow* parent, std::shared_ptr<cfg::Configuration> config, std::shared_ptr<spdlog::logger> logger, std::shared_ptr<services::TaskStateService> taskState, const wxString& name = wxT("TimedTaskDialog"));
    explicit TimedTaskDialog(wxWindow* parent, std::shared_ptr<cfg::Configuration> config, std::shared_ptr<spdlog::logger> logger, std::shared_ptr<services::TaskStateService> taskState, bool hasPendingPausedTask, const wxString& name = wxT("TimedTaskDialog"));
    virtual ~TimedTaskDialog() = default;

    void Launch();
    void LaunchInPausedState();

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name);

    void CreateControls();

    void OnElapsedTimeUpdate(wxTimerEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnHideWindow(wxTimerEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    std::shared_ptr<spdlog::logger> pLogger;
    wxWindow* pParent;
    wxStaticText* pElapsedTimeText;
    wxStaticText* pAccumulatedTimeText;
    wxCheckBox* pStartNewTask;
    wxButton* pStartButton;
    wxButton* pPauseButton;
    wxButton* pStopButton;
    wxButton* pCancelButton;
    std::unique_ptr<wxTimer> pElapsedTimer;
    std::unique_ptr<wxTimer> pNotificationTimer;
    std::unique_ptr<wxTimer> pHideWindowTimer;
    std::shared_ptr<cfg::Configuration> pConfig;
    std::shared_ptr<services::TaskStateService> pTaskState;

    wxDateTime mStartTime;
    wxDateTime mEndTime;
    bool bWasTaskPaused;
    bool bHasPendingPausedTask;

    enum
    {
        IDC_ELAPSED = wxID_HIGHEST + 1,
        IDC_ACCUMULATED_TIME,
        IDC_START_NEW_TASK_CHECK,
        IDC_ELAPSED_TIMER,
        IDC__NOTIFICATION_TIMER,
        IDC_HIDE_WINDOW_TIMER,
        IDC_START,
        IDC_PAUSE,
        IDC_STOP,
        IDC_CANCEL
    };
};
} // app::dialog
