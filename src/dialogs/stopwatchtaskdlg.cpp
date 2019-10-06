//  Tasks Tracker is a desktop that aids you in tracking your timesheets
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

#include "stopwatchtaskdlg.h"

#include <wx/notifmsg.h>
#include <wx/statline.h>

#include "../common/common.h"
#include "../common/util.h"
#include "../services/taskstateservice.h"
#include "taskitemdlg.h"

wxDEFINE_EVENT(START_NEW_STOPWATCH_TASK, wxCommandEvent);

namespace app::dialog
{
static const wxString ElapsedTimeText = wxT("Elapsed Time: %s");
static const wxString AccumulatedTimeText = wxT("Time accumulated thus far: %s");

// clang-format off
wxBEGIN_EVENT_TABLE(StopwatchTaskDialog, wxDialog)
EVT_CLOSE(StopwatchTaskDialog::OnClose)
EVT_TIMER(StopwatchTaskDialog::IDC_NOTIFICATION_TIMER, StopwatchTaskDialog::OnTimer)
EVT_TIMER(StopwatchTaskDialog::IDC_ELAPSED_TIMER, StopwatchTaskDialog::OnElapsedTimeUpdate)
EVT_TIMER(StopwatchTaskDialog::IDC_HIDE_WINDOW_TIMER, StopwatchTaskDialog::OnHideWindow)
EVT_TIMER(StopwatchTaskDialog::IDC_PAUSED_TASK_REMINDER, StopwatchTaskDialog::OnPausedTaskReminder)
EVT_BUTTON(StopwatchTaskDialog::IDC_START, StopwatchTaskDialog::OnStart)
EVT_BUTTON(StopwatchTaskDialog::IDC_PAUSE, StopwatchTaskDialog::OnPause)
EVT_BUTTON(StopwatchTaskDialog::IDC_STOP, StopwatchTaskDialog::OnStop)
EVT_BUTTON(StopwatchTaskDialog::IDC_CANCEL, StopwatchTaskDialog::OnCancel)
wxEND_EVENT_TABLE()

StopwatchTaskDialog::StopwatchTaskDialog(wxWindow* parent,
    std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<services::TaskStateService> taskState,
    const wxString& name)
    : pLogger(logger)
    , pParent(parent)
    , pElapsedTimeText(nullptr)
    , pElapsedTimer(std::make_unique<wxTimer>(this, IDC_ELAPSED_TIMER))
    , pNotificationTimer(std::make_unique<wxTimer>(this, IDC_NOTIFICATION_TIMER))
    , pHideWindowTimer(std::make_unique<wxTimer>(this, IDC_HIDE_WINDOW_TIMER))
    , pPausedTaskReminder(std::make_unique<wxTimer>(this, IDC_PAUSED_TASK_REMINDER))
    , pConfig(config)
    , pTaskState(taskState)
    , mStartTime()
    , bWasTaskPaused(false)
    , bHasPendingPausedTask(false)
// clang-format on
{
    Create(parent,
        wxID_ANY,
        wxT("Stopwatch Task"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

StopwatchTaskDialog::StopwatchTaskDialog(wxWindow* parent,
    std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<services::TaskStateService> taskState,
    bool hasPendingPausedTask,
    const wxString& name)
    : pLogger(logger)
    , pParent(parent)
    , pElapsedTimeText(nullptr)
    , pElapsedTimer(std::make_unique<wxTimer>(this, IDC_ELAPSED_TIMER))
    , pNotificationTimer(std::make_unique<wxTimer>(this, IDC_NOTIFICATION_TIMER))
    , pHideWindowTimer(std::make_unique<wxTimer>(this, IDC_HIDE_WINDOW_TIMER))
    , pPausedTaskReminder(std::make_unique<wxTimer>(this, IDC_PAUSED_TASK_REMINDER))
    , pConfig(config)
    , pTaskState(taskState)
    , mStartTime()
    , bWasTaskPaused(false)
    , bHasPendingPausedTask(hasPendingPausedTask)
{
    Create(parent,
        wxID_ANY,
        wxT("Timed Task"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

void StopwatchTaskDialog::Launch()
{
    mStartTime = wxDateTime::Now();
    pElapsedTimer->Start(1000 /*milliseconds*/);
    pNotificationTimer->Start(util::MinutesToMilliseconds(pConfig->GetNotificationTimerInterval()));

    if (pConfig->IsMinimizeTimedTaskWindow()) {
        pHideWindowTimer->Start(util::SecondsToMilliseconds(pConfig->GetHideWindowTimerInterval()), true);
    }

    pStartButton->Disable();
    if (bHasPendingPausedTask) {
        pStartNewTask->Disable();
    }

    wxDialog::ShowModal();
}

void StopwatchTaskDialog::LaunchInPausedState()
{
    pStartButton->Enable();
    pPauseButton->Disable();
    pStartNewTask->Disable();
    bWasTaskPaused = true;

    auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
    pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));

    wxDialog::ShowModal();
}

bool StopwatchTaskDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

void StopwatchTaskDialog::CreateControls()
{
    /* Sizer and Panel Control */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this, wxID_STATIC);
    mainPanel->SetSizer(sizer);
    mainSizer->Add(mainPanel, common::sizers::ControlExpand);

    /* Elapsed Time Text Control */
    pElapsedTimeText = new wxStaticText(mainPanel, IDC_ELAPSED, wxT("Elapsed Time: 00:00:00"));
    auto font = pElapsedTimeText->GetFont();
    font.SetPointSize(16);
    pElapsedTimeText->SetFont(font);
    sizer->Add(pElapsedTimeText, common::sizers::ControlCenterHorizontal);

    pAccumulatedTimeText =
        new wxStaticText(mainPanel, IDC_ACCUMULATED_TIME, wxT("Time accumulated thus far: 00:00:00"));
    auto font2 = pAccumulatedTimeText->GetFont();
    font2.SetPointSize(8);
    pAccumulatedTimeText->SetFont(font2);
    sizer->Add(pAccumulatedTimeText, common::sizers::ControlCenterHorizontal);

    pStartNewTask =
        new wxCheckBox(mainPanel, IDC_START_NEW_TASK_CHECK, wxT("Start new task when pausing current task"));
    sizer->Add(pStartNewTask, common::sizers::ControlDefault);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    pStartButton = new wxButton(buttonPanel, IDC_START, wxT("St&art"));
    pPauseButton = new wxButton(buttonPanel, IDC_PAUSE, wxT("&Pause"));
    pStopButton = new wxButton(buttonPanel, IDC_STOP, wxT("&Stop"));
    pStopButton->SetFocus();
    pStopButton->SetDefault();
    pCancelButton = new wxButton(buttonPanel, IDC_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pStartButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pPauseButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pStopButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pCancelButton, common::sizers::ControlDefault);
}

void StopwatchTaskDialog::OnElapsedTimeUpdate(wxTimerEvent& WXUNUSED(event))
{
    auto current = wxDateTime::Now();
    auto timeDiff = current - mStartTime;
    pElapsedTimeText->SetLabel(wxString::Format(ElapsedTimeText, timeDiff.Format()));
}

void StopwatchTaskDialog::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    auto current = wxDateTime::Now();
    auto elapsed = current - mStartTime;
    auto message = wxString::Format(wxT("Task running for: %s"), elapsed.Format());
    wxNotificationMessage taskElaspedMessage(common::GetProgramName(), message, this);
    taskElaspedMessage.Show();
}

void StopwatchTaskDialog::OnHideWindow(wxTimerEvent& WXUNUSED(event))
{
    Iconize(true);
    pHideWindowTimer->Stop();
}

void StopwatchTaskDialog::OnPausedTaskReminder(wxTimerEvent& event)
{
    auto message = wxT("There is a pending pasued task");
    wxNotificationMessage pausedTaskMessage(common::GetProgramName(), message, this);
    pausedTaskMessage.Show();
}

void StopwatchTaskDialog::OnStart(wxCommandEvent& WXUNUSED(event))
{
    bIsPaused = false;
    mStartTime = wxDateTime::Now();

    pElapsedTimer->Start(1000 /*milliseconds*/);
    pNotificationTimer->Start(util::MinutesToMilliseconds(pConfig->GetNotificationTimerInterval()));
    pPausedTaskReminder->Stop();

    pStopButton->Enable();
    pStartButton->Disable();
    pPauseButton->Enable();
    if (bHasPendingPausedTask) {
        pStartNewTask->Disable();
    } else {
        pStartNewTask->Enable();
    }
}

void StopwatchTaskDialog::OnPause(wxCommandEvent& WXUNUSED(event))
{
    pStartButton->Enable();
    pPauseButton->Disable();
    pStartNewTask->Disable();
    pNotificationTimer->Stop();
    pElapsedTimer->Stop();
    pPausedTaskReminder->Start(util::MinutesToMilliseconds(pConfig->GetPausedTaskReminderInterval()));
    bWasTaskPaused = true;
    bIsPaused = true;

    mEndTime = wxDateTime::Now();
    pTaskState->PushTimes(mStartTime, mEndTime);
    auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
    pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));

    if (pStartNewTask->IsChecked()) {
        wxCommandEvent startNewStopwatchTask(START_NEW_STOPWATCH_TASK);
        wxPostEvent(pParent, startNewStopwatchTask);
        EndModal(wxID_OK);
    }
}

void StopwatchTaskDialog::OnStop(wxCommandEvent& WXUNUSED(event))
{
    mEndTime = wxDateTime::Now();
    pNotificationTimer->Stop();
    pElapsedTimer->Stop();
    if (pPausedTaskReminder->IsRunning()) {
        pPausedTaskReminder->Stop();
    }
    pStopButton->Disable();
    pPauseButton->Disable();
    pStartButton->Disable();
    pStartNewTask->Disable();

    if (bWasTaskPaused) {
        if (!bIsPaused) {
            pTaskState->PushTimes(mStartTime, mEndTime);
        }

        auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
        pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));

        auto durationOfTask = pTaskState->GetAccumulatedTime();

        dialog::TaskItemDialog newTask(this->GetParent(), pLogger, pConfig, TaskItemType::EntryTask);
        newTask.SetDurationFromStopwatchTask(durationOfTask);
        newTask.ShowModal();
    } else {
        dialog::TaskItemDialog newTask(this->GetParent(), pLogger, pConfig, TaskItemType::TimedTask);
        newTask.SetTimesFromStopwatchTask(mStartTime, mEndTime);
        newTask.ShowModal();
    }

    EndModal(wxID_OK);
}

void StopwatchTaskDialog::OnCancel(wxCommandEvent& event)
{
    pTaskState->mTimes.clear();
    EndModal(wxID_CANCEL);
}

void StopwatchTaskDialog::OnClose(wxCloseEvent& event)
{
    pTaskState->mTimes.clear();
    EndModal(wxID_CLOSE);
}

} // namespace app::dialog
