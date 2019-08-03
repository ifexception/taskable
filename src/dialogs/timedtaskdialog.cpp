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

#include "timedtaskdialog.h"

#include <wx/notifmsg.h>
#include <wx/statline.h>

#include "../common/common.h"
#include "../common/util.h"
#include "../services/taskstateservice.h"
#include "taskitemdialog.h"

wxDEFINE_EVENT(START_NEW_TIMED_TASK, wxCommandEvent);

namespace app::dialog
{
static const wxString ElapsedTimeText = wxT("Elapsed Time: %s");
static const wxString AccumulatedTimeText = wxT("Time accumulated thus far: %s");

wxBEGIN_EVENT_TABLE(TimedTaskDialog, wxDialog)
EVT_CLOSE(TimedTaskDialog::OnClose)
EVT_TIMER(TimedTaskDialog::IDC__NOTIFICATION_TIMER, TimedTaskDialog::OnTimer)
EVT_TIMER(TimedTaskDialog::IDC_ELAPSED_TIMER, TimedTaskDialog::OnElapsedTimeUpdate)
EVT_TIMER(TimedTaskDialog::IDC_HIDE_WINDOW_TIMER, TimedTaskDialog::OnHideWindow)
EVT_BUTTON(TimedTaskDialog::IDC_START, TimedTaskDialog::OnStart)
EVT_BUTTON(TimedTaskDialog::IDC_PAUSE, TimedTaskDialog::OnPause)
EVT_BUTTON(TimedTaskDialog::IDC_STOP, TimedTaskDialog::OnStop)
wxEND_EVENT_TABLE()

TimedTaskDialog::TimedTaskDialog(wxWindow* parent, std::shared_ptr<cfg::Configuration> config, std::shared_ptr<spdlog::logger> logger, std::shared_ptr<services::TaskStateService> taskState, const wxString& name)
    : pLogger(logger)
    , pParent(parent)
    , pElapsedTimeText(nullptr)
    , pElapsedTimer(std::make_unique<wxTimer>(this, IDC_ELAPSED_TIMER))
    , pNotificationTimer(std::make_unique<wxTimer>(this, IDC__NOTIFICATION_TIMER))
    , pHideWindowTimer(std::make_unique<wxTimer>(this, IDC_HIDE_WINDOW_TIMER))
    , pConfig(config)
    , pTaskState(taskState)
    , mStartTime()
    , bWasTaskPaused(false)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    Create(parent, wxID_ANY, wxT("Timed Task"), wxDefaultPosition, wxSize(320, 240), style, name);
}

void TimedTaskDialog::Launch()
{
    mStartTime = wxDateTime::Now();
    pElapsedTimer->Start(1000/*milliseconds*/);
    pNotificationTimer->Start(util::MinutesToMilliseconds(pConfig->GetNotificationTimerInterval()));

    if (pConfig->IsMinimizeTimedTaskWindow()) {
        pHideWindowTimer->Start(util::SecondsToMilliseconds(pConfig->GetHideWindowTimerInterval()), true);
    }
    pStartButton->Disable();
    wxDialog::ShowModal();
}

bool TimedTaskDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
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

void TimedTaskDialog::CreateControls()
{
    /* Sizer and Panel Control */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this, wxID_STATIC);
    mainPanel->SetSizer(sizer);
    mainSizer->Add(mainPanel, common::sizers::ControlDefault);

    /* Elapsed Time Text Control */
    pElapsedTimeText = new wxStaticText(mainPanel, IDC_ELAPSED, wxT("Elapsed Time: 00:00:00"));
    auto font = pElapsedTimeText->GetFont();
    font.SetPointSize(16);
    pElapsedTimeText->SetFont(font);
    sizer->Add(pElapsedTimeText, common::sizers::ControlCenter);

    pAccumulatedTimeText = new wxStaticText(mainPanel, IDC_ACCUMULATED_TIME, wxT("Time accumulatedthus far: 00:00:00"));
    auto font2 = pAccumulatedTimeText->GetFont();
    font2.SetPointSize(8);
    pAccumulatedTimeText->SetFont(font2);
    sizer->Add(pAccumulatedTimeText, common::sizers::ControlRight);

    pStartNewTask = new wxCheckBox(mainPanel, IDC_START_NEW_TASK_CHECK, wxT("Start new task when pausing current task"));
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
    pCancelButton = new wxButton(buttonPanel, IDC_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pStartButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pPauseButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pStopButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pCancelButton, common::sizers::ControlDefault);
}

void TimedTaskDialog::OnElapsedTimeUpdate(wxTimerEvent& WXUNUSED(event))
{
    auto current = wxDateTime::Now();
    auto timeDiff = current - mStartTime;
    pElapsedTimeText->SetLabel(wxString::Format(ElapsedTimeText, timeDiff.Format()));
}

void TimedTaskDialog::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    auto current = wxDateTime::Now();
    auto elapsed = current - mStartTime;
    auto message = wxString::Format(wxT("Timed Task running for: %s"), elapsed.Format());
    wxNotificationMessage taskElaspedMessage(wxT("Task Tracker"), message, this);
    taskElaspedMessage.Show();
}

void TimedTaskDialog::OnHideWindow(wxTimerEvent& WXUNUSED(event))
{
    Iconize(true);
    pHideWindowTimer->Stop();
}

void TimedTaskDialog::OnStart(wxCommandEvent& WXUNUSED(event))
{
    mStartTime = wxDateTime::Now();
    pNotificationTimer->Start();
    pElapsedTimer->Start();
    pStopButton->Enable();
    pStartButton->Disable();
    pPauseButton->Enable();
    pStartNewTask->Enable();
}

void TimedTaskDialog::OnPause(wxCommandEvent& WXUNUSED(event))
{
    pStartButton->Enable();
    pPauseButton->Disable();
    pStartNewTask->Disable();
    pNotificationTimer->Stop();
    pElapsedTimer->Stop();
    bWasTaskPaused = true;
    mEndTime = wxDateTime::Now();

    auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
    pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));
    pTaskState->PushTimes(mStartTime, mEndTime);

    if (pStartNewTask->IsChecked()) {
        wxCommandEvent startNewTimedTask(START_NEW_TIMED_TASK);
        wxPostEvent(pParent, startNewTimedTask);
        EndModal(wxID_OK);
    }
}

void TimedTaskDialog::OnStop(wxCommandEvent& WXUNUSED(event))
{
    mEndTime = wxDateTime::Now();
    pNotificationTimer->Stop();
    pElapsedTimer->Stop();
    pStopButton->Disable();
    pPauseButton->Disable();
    pStartButton->Disable();

    if (bWasTaskPaused) {
        pTaskState->PushTimes(mStartTime, mEndTime);

        auto accumulatedTimeThusFar = pTaskState->GetAccumulatedTime();
        pAccumulatedTimeText->SetLabel(wxString::Format(AccumulatedTimeText, accumulatedTimeThusFar.Format()));

        auto durationOfTask = pTaskState->GetAccumulatedTime();

        dialog::TaskItemDialog newTask(this->GetParent(), pLogger, durationOfTask);
        newTask.ShowModal();
    } else {
        dialog::TaskItemDialog newTask(this->GetParent(), pLogger, mStartTime, mEndTime);
        newTask.ShowModal();
    }

    EndModal(wxID_OK);
}

void TimedTaskDialog::OnCancel(wxCommandEvent& event)
{
    pTaskState->Clear();
    EndModal(wxID_CANCEL);
}

void TimedTaskDialog::OnClose(wxCloseEvent& event)
{
    pTaskState->Clear();
}

} // app::dialog
