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

#include <wx/statline.h>

#include "../common/common.h"
#include "../common/ids.h"

namespace app::dialog
{
static const wxString ElapsedTimeText = wxT("Elapsed Time: %s");

wxBEGIN_EVENT_TABLE(TimedTaskDialog, wxDialog)
EVT_TIMER(TimedTaskDialog::IDC_TIMER, TimedTaskDialog::OnTimer)
EVT_TIMER(TimedTaskDialog::IDC_HIDE_WINDOW_TIMER, TimedTaskDialog::OnHideWindow)
EVT_TIMER(TimedTaskDialog::IDC_ELAPSED_TIMER, TimedTaskDialog::OnElapsedTimeUpdate)
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
    SetMinClientSize(wxSize(320, 240));
}

void TimedTaskDialog::Show()
{
    mStartTime = wxDateTime::Now();
    pElapsedTimer->Start(1000);
    pTimer->Start(1800000);

    bIsRunning = true;
    wxTopLevelWindow::Iconize(true);
}

bool TimedTaskDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
{
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (created) {
        CreateControls();
        SetIcon(common::GetProgramIcon());
        GetSizer()->Fit(this);
        CenterOnScreen();
    }

    return created;
}

void TimedTaskDialog::CreateControls()
{
    /* Sizer and Panel Control */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this);
    mainPanel->SetSizer(mainSizer);

    /* Elapsed Time Text Control */
    pElapsedTimeText = new wxStaticText(mainPanel, IDC_ELAPSED, wxT("Elapsed Time: %s"));
    auto font = pElapsedTimeText->GetFont();
    font.SetPointSize(16);
    pElapsedTimeText->SetFont(font);
    pElapsedTimeText->CenterOnParent();

    mainSizer->Add(pElapsedTimeText, common::sizers::ControlCenter);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    auto startButton = new wxButton(buttonPanel, IDC_START, wxT("St&art"));
    auto stopButton = new wxButton(buttonPanel, ids::ID_STOP, wxT("&Stop"));

    buttonPanelSizer->Add(startButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(stopButton, common::sizers::ControlDefault);
}

void TimedTaskDialog::OnElapsedTimeUpdate(wxTimerEvent& event)
{
    auto current = wxDateTime::Now();
    auto timeDiff = current - mStartTime;
    pElapsedTimeText->SetLabel(wxString::Format(ElapsedTimeText, timeDiff.Format()));
}

void TimedTaskDialog::OnTimer(wxTimerEvent& event)
{
    if (!IsShownOnScreen()) {
        Restore();
        Raise();
        wxTopLevelWindowMSW::Show();
    }

    pHideWindowTimer->Start(2000);
}

void TimedTaskDialog::OnHideWindow(wxTimerEvent& event)
{
    pHideWindowTimer->Stop();
    wxTopLevelWindow::Iconize(true);
}

void TimedTaskDialog::OnRestart(wxCommandEvent& event)
{
    bIsRunning = true;
    bIsPaused = false;
}

void TimedTaskDialog::OnPause(wxCommandEvent& event)
{
    bIsRunning = false;
    bIsPaused = true;

    // TODO Handle multiple pauses, how to log all the time? Is this a requirement?
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
