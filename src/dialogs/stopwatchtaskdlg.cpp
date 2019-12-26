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
static const wxString TaskRunningForText = wxT("Task running for: %s");
static const wxString PendingPausedTaskText = wxT("There is a pending pasued task");

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
    frame::TaskBarIcon *taskBarIcon,
    const wxString& name)
    : pLogger(logger)
    , pParent(parent)
    , pElapsedTimeText(nullptr)
    , pAccumulatedTimeText(nullptr)
    , pStartNewTask(nullptr)
    , pStopwatchDescription(nullptr)
    , pStartButton(nullptr)
    , pPauseButton(nullptr)
    , pStopButton(nullptr)
    , pCancelButton(nullptr)
    , pElapsedTimer(std::make_unique<wxTimer>(this, IDC_ELAPSED_TIMER))
    , pNotificationTimer(std::make_unique<wxTimer>(this, IDC_NOTIFICATION_TIMER))
    , pHideWindowTimer(std::make_unique<wxTimer>(this, IDC_HIDE_WINDOW_TIMER))
    , pPausedTaskReminder(std::make_unique<wxTimer>(this, IDC_PAUSED_TASK_REMINDER))
    , pConfig(config)
    , pTaskState(taskState)
    , pTaskBarIcon(taskBarIcon)
    , mStartTime(wxDefaultDateTime)
    , mEndTime(wxDefaultDateTime)
    , bWasTaskPaused(false)
    , bHasPendingPausedTask(false)
// clang-format on
{
    Create(parent,
        wxID_ANY,
        wxT("Stopwatch"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

StopwatchTaskDialog::StopwatchTaskDialog(wxWindow* parent,
    std::shared_ptr<cfg::Configuration> config,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<services::TaskStateService> taskState,
    frame::TaskBarIcon *taskBarIcon,
    bool hasPendingPausedTask,
    const wxString& name)
    : pLogger(logger)
    , pParent(parent)
    , pElapsedTimeText(nullptr)
    , pAccumulatedTimeText(nullptr)
    , pStartNewTask(nullptr)
    , pStopwatchDescription(nullptr)
    , pStartButton(nullptr)
    , pPauseButton(nullptr)
    , pStopButton(nullptr)
    , pCancelButton(nullptr)
    , pElapsedTimer(std::make_unique<wxTimer>(this, IDC_ELAPSED_TIMER))
    , pNotificationTimer(std::make_unique<wxTimer>(this, IDC_NOTIFICATION_TIMER))
    , pHideWindowTimer(std::make_unique<wxTimer>(this, IDC_HIDE_WINDOW_TIMER))
    , pPausedTaskReminder(std::make_unique<wxTimer>(this, IDC_PAUSED_TASK_REMINDER))
    , pConfig(config)
    , pTaskState(taskState)
    , pTaskBarIcon(taskBarIcon)
    , mStartTime(wxDefaultDateTime)
    , mEndTime(wxDefaultDateTime)
    , bWasTaskPaused(false)
    , bHasPendingPausedTask(hasPendingPausedTask)
{
    Create(parent,
        wxID_ANY,
        wxT("Stopwatch"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

void StopwatchTaskDialog::Launch()
{
    pPauseButton->Disable();
    pStopButton->Disable();
    pStartNewTask->Disable();

    if (pConfig->IsStartStopwatchOnLaunch()) {
        ExecuteStartupProcedure();
    }

    wxDialog::ShowModal();
}

void StopwatchTaskDialog::Relaunch()
{
    // if we can start stopwatch on resume, then execute startup procedure
    if (pConfig->IsStartStopwatchOnResume()) {
        ExecuteStartupProcedure();
    } else {
        // if we cannot, enable start button
        pStartButton->Enable();

        // and disable pause button
        pPauseButton->Disable();
        pStartNewTask->Disable();
    }

    /* set state */
    bWasTaskPaused = true;

    /* restore state */
    auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
    pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));
    if (!pTaskState->GetStoredDescription().empty()) {
        pStopwatchDescription->ChangeValue(pTaskState->GetStoredDescription());
    }

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
        GetSizer()->SetSizeHints(this);
        SetIcon(common::GetProgramIcon());
        Center();

        if (pConfig->IsShowInTray()) {
            wxNotificationMessage::UseTaskBarIcon(pTaskBarIcon);
        } else {
            // Need to implement installing application with shortcut
            // wxNotificationMessage::MSWUseToasts(wxGetEmptyString(), wxGetEmptyString());
        }
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

    /* Accumulated Time Text Control */
    pAccumulatedTimeText =
        new wxStaticText(mainPanel, IDC_ACCUMULATED_TIME, wxT("Time accumulated thus far: 00:00:00"));
    auto font2 = pAccumulatedTimeText->GetFont();
    font2.SetPointSize(8);
    pAccumulatedTimeText->SetFont(font2);
    sizer->Add(pAccumulatedTimeText, common::sizers::ControlCenterHorizontal);

    /* Start New Task on Pause Checkbox Control */
    pStartNewTask =
        new wxCheckBox(mainPanel, IDC_START_NEW_TASK_CHECK, wxT("Start new task when pausing current task"));
    sizer->Add(pStartNewTask, common::sizers::ControlDefault);

    sizer->AddSpacer(2);

    /* Task Description Sizer */
    auto taskDescSizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(taskDescSizer, common::sizers::ControlExpand);

    /* Stopwatch Task Description Text Control */
    auto taskDescriptionText = new wxStaticText(mainPanel, wxID_STATIC, wxT("Description"));
    taskDescSizer->Add(taskDescriptionText, common::sizers::ControlCenter);

    pStopwatchDescription =
        new wxTextCtrl(mainPanel, IDC_TASK_DESCRIPTION, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1));
    pStopwatchDescription->SetToolTip(wxT("Enter a short description for the stopwatch task to track"));
    pStopwatchDescription->SetMaxLength(32);
    taskDescSizer->Add(pStopwatchDescription, common::sizers::ControlDefault);

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

void StopwatchTaskDialog::ExecuteStartupProcedure()
{
    /* get the current time */
    mStartTime = wxDateTime::Now();

    /* set state */
    bIsPaused = false;

    /* start the timers */
    pElapsedTimer->Start(1000 /*milliseconds*/);
    pNotificationTimer->Start(util::MinutesToMilliseconds(pConfig->GetNotificationTimerInterval()));

    /* stop paused task reminder */
    if (pPausedTaskReminder->IsRunning()) {
        pPausedTaskReminder->Stop();
    }

    /* enable stop and pause buttons */
    pStopButton->Enable();
    pPauseButton->Enable();

    /* disable start button */
    pStartButton->Disable();

    /* check for pending task */
    if (bHasPendingPausedTask) {
        pStartNewTask->Disable();
    } else {
        pStartNewTask->Enable();
    }
}

void StopwatchTaskDialog::ExecutePauseProcedure()
{
    /* set state */
    bIsPaused = true;
    bWasTaskPaused = true;

    /* enable start button */
    pStartButton->Enable();

    /* disable pause button */
    pPauseButton->Disable();

    /* disable checkbox to start new task */
    pStartNewTask->Disable();

    /* stop timers */
    pNotificationTimer->Stop();
    pElapsedTimer->Stop();

    /* start timer */
    pPausedTaskReminder->Start(util::MinutesToMilliseconds(pConfig->GetPausedTaskReminderInterval()));

    /* get the current end time */
    mEndTime = wxDateTime::Now();

    /* save state */
    pTaskState->PushTimes(mStartTime, mEndTime);
    if (!pStopwatchDescription->GetValue().empty()) {
        if (pStopwatchDescription->GetValue() != pTaskState->GetStoredDescription()) {
            pTaskState->StoreDescription(pStopwatchDescription->GetValue());
        }
    }

    /* update UI */
    auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
    pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));

    /* check if a new stopwatch task needs to be started */
    if (pStartNewTask->IsChecked()) {
        wxCommandEvent startNewStopwatchTask(START_NEW_STOPWATCH_TASK);
        wxPostEvent(pParent, startNewStopwatchTask);
        EndModal(wxID_OK);
    }
}

void StopwatchTaskDialog::ExecuteStopProcedure()
{
    /* did the user go from pause to stop state? */
    if (!bIsPaused) {
        /* get the current end time */
        mEndTime = wxDateTime::Now();

        /* push state */
        pTaskState->PushTimes(mStartTime, mEndTime);
    }

    /* stop timers */
    pNotificationTimer->Stop();
    pElapsedTimer->Stop();
    if (pPausedTaskReminder->IsRunning()) {
        pPausedTaskReminder->Stop();
    }

    /* disable buttons */
    pStopButton->Disable();
    pPauseButton->Disable();
    pStartButton->Disable();
    pStartNewTask->Disable();

    /* was the task paused previously? */
    if (bWasTaskPaused) {
        /* save state */
        auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
        pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));

        /* sum up the time for the task */
        auto durationOfTask = pTaskState->GetAccumulatedTime();

        /* launch dialog */
        dialog::TaskItemDialog newTask(this->GetParent(), pLogger, pConfig, constants::TaskItemTypes::EntryTask);
        newTask.SetDurationFromStopwatchTask(durationOfTask);
        newTask.SetDescriptionFromStopwatchTask(pStopwatchDescription->GetValue());
        newTask.ShowModal();
    } else {
        /* launch dialog */
        dialog::TaskItemDialog newTask(this->GetParent(), pLogger, pConfig, constants::TaskItemTypes::TimedTask);
        newTask.SetTimesFromStopwatchTask(mStartTime, mEndTime);
        newTask.SetDescriptionFromStopwatchTask(pStopwatchDescription->GetValue());
        newTask.ShowModal();
    }
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
    auto message = wxString::Format(TaskRunningForText, elapsed.Format());

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
    wxNotificationMessage pausedTaskMessage(common::GetProgramName(), PendingPausedTaskText, this);
    pausedTaskMessage.Show();
}

void StopwatchTaskDialog::OnStart(wxCommandEvent& WXUNUSED(event))
{
    ExecuteStartupProcedure();
}

void StopwatchTaskDialog::OnPause(wxCommandEvent& WXUNUSED(event))
{
    ExecutePauseProcedure();
}

void StopwatchTaskDialog::OnStop(wxCommandEvent& WXUNUSED(event))
{
    ExecuteStopProcedure();

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
