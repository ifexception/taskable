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

namespace app::dialog
{
static const wxString ElapsedTimeText = wxT("Elapsed Time: %s");

wxBEGIN_EVENT_TABLE(TimedTaskDialog, wxDialog)
EVT_TIMER(TimedTaskDialog::IDC_TIMER, TimedTaskDialog::OnTimer)
EVT_TIMER(TimedTaskDialog::IDC_ELAPSED_TIMER, TimedTaskDialog::OnElapsedTimeUpdate)
EVT_TIMER(TimedTaskDialog::IDC_HIDE_WINDOW_TIMER, TimedTaskDialog::OnHideWindow)
EVT_BUTTON(TimedTaskDialog::IDC_STOP, TimedTaskDialog::OnStop)
wxEND_EVENT_TABLE()

TimedTaskDialog::TimedTaskDialog(wxWindow* parent, const wxString& name)
    : pParent(parent)
    , pElapsedTimeText(nullptr)
    , pElapsedTimer(std::make_unique<wxTimer>(this, IDC_ELAPSED_TIMER))
    , pTimer(std::make_unique<wxTimer>(this, IDC_TIMER))
    , pHideWindowTimer(std::make_unique<wxTimer>(this, IDC_HIDE_WINDOW_TIMER))
    , mStartTime()
    , bIsRunning(false)
    , bIsPaused(false)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    Create(parent, wxID_ANY, wxT("Timed Task"), wxDefaultPosition, wxSize(320, 240), style, name);
}

void TimedTaskDialog::Launch()
{
    mStartTime = wxDateTime::Now();
    pElapsedTimer->Start(1000);
    pTimer->Start(/*1800000*/10000);

    bIsRunning = true;
    pHideWindowTimer->Start(2000, true);
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
    mainSizer->Add(sizer, 0);

    auto mainPanel = new wxPanel(this, wxID_STATIC);
    mainPanel->SetSizer(sizer);
    mainSizer->Add(mainPanel, common::sizers::ControlDefault);

    /* Elapsed Time Text Control */
    pElapsedTimeText = new wxStaticText(mainPanel, IDC_ELAPSED, wxT("Elapsed Time: 00:00:00"));
    auto font = pElapsedTimeText->GetFont();
    font.SetPointSize(16);
    pElapsedTimeText->SetFont(font);
    sizer->Add(pElapsedTimeText, common::sizers::ControlCenter);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    auto startButton = new wxButton(buttonPanel, IDC_START, wxT("St&art"));
    startButton->Disable();
    auto stopButton = new wxButton(buttonPanel, IDC_STOP, wxT("&Stop"));

    buttonPanelSizer->Add(startButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(stopButton, common::sizers::ControlDefault);
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
    wxLogDebug("OnHideWindow");
    Iconize(true);
    pHideWindowTimer->Stop();
}

void TimedTaskDialog::OnStop(wxCommandEvent& event)
{
    mEndTime = wxDateTime::Now();
    bIsRunning = false;
    pTimer->Stop();
    pElapsedTimer->Stop();

    // TODO Start taskitem dialog
    // TODO Pass along the start and end time to pre-populate the dialog
}

} // app::dialog
