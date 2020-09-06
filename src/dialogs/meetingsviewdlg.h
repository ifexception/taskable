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

#include <spdlog/spdlog.h>
#include <wx/wx.h>
#include <wx/activityindicator.h>
#include <wx/thread.h>

#include "../models/meetingmodel.h"
#include "../services/outlookintegrator.h"

/*
 Run Outlook integrator in a thread
 Display of all meetings in a scrolled window
*/

wxDECLARE_EVENT(GET_MEETINGS_THREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(GET_MEETINGS_THREAD_ERROR, wxThreadEvent);

namespace app::dlg
{
class MeetingsViewDialog;

class GetMeetingsThread : public wxThread
{
public:
    GetMeetingsThread() = delete;
    GetMeetingsThread(MeetingsViewDialog* handler);
    virtual ~GetMeetingsThread();

protected:
    ExitCode Entry() override;
    void OnExit() override;

private:
    MeetingsViewDialog* pHandler;
};

class MeetingsViewDialog : public wxDialog
{
public:
    MeetingsViewDialog() = default;
    MeetingsViewDialog(wxWindow* parent,
        std::shared_ptr<spdlog::logger> logger,
        const wxString& name = "meetingsviewdlg");
    virtual ~MeetingsViewDialog();

    void LaunchModeless();

    void GetMeetingDataFromThread(std::vector<svc::Meeting*> meetings);

protected:
    GetMeetingsThread* pThread;
    wxCriticalSection mCriticalSection;

private:
    bool Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();
    void ConfigureEventBindings();
    void FillControls();

    void AppendMeetingControls(svc::Meeting* meeting);

    void StartThread();

    void ThreadCleanupProcedure();

    void OnClose(wxCloseEvent& event);
    void OnThreadCompletion(wxThreadEvent& event);
    void OnThreadError(wxThreadEvent& event);
    void OnAttendedCheckboxCheck(wxCommandEvent& event);

    friend class GetMeetingsThread;

    std::shared_ptr<spdlog::logger> pLogger;

    wxScrolledWindow* pScrolledWindow;
    wxStaticText* pTodayDateLabel;
    wxActivityIndicator* pActivityIndicator;

    std::vector<svc::Meeting*> mMeetings;

    enum { IDC_TODAYDATE = wxID_HIGHEST + 1, IDC_ACTIVITYINDICATOR };
};
} // namespace app::dlg
