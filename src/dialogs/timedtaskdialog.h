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

namespace app::dialog
{
class TimedTaskDialog : public wxDialog
{
public:
    TimedTaskDialog() = delete;
    explicit TimedTaskDialog(wxWindow* parent, const wxString& name = wxT("TimedTaskDialog"));
    virtual ~TimedTaskDialog() = default;

    void Launch();

private:
    wxDECLARE_EVENT_TABLE();

    bool Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name);

    void CreateControls();

    void OnElapsedTimeUpdate(wxTimerEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnHideWindow(wxTimerEvent& event);
    void OnStop(wxCommandEvent& event);

    wxWindow* pParent;
    wxStaticText* pElapsedTimeText;
    wxButton* pStartButton;
    wxButton* pStopButton;
    std::unique_ptr<wxTimer> pElapsedTimer;
    std::unique_ptr<wxTimer> pTimer;
    std::unique_ptr<wxTimer> pHideWindowTimer;

    wxDateTime mStartTime;
    wxDateTime mEndTime;
    bool bIsRunning;
    bool bIsPaused;

    enum
    {
        IDC_ELAPSED = wxID_HIGHEST + 1,
        IDC_ELAPSED_TIMER,
        IDC_TIMER,
        IDC_HIDE_WINDOW_TIMER,
        IDC_START,
        IDC_STOP,
    };
};
} // app::dialog
