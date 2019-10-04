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

#pragma once

#include <wx/wx.h>
#include <wx/hyperlink.h>
#include <wx/thread.h>

wxDECLARE_EVENT(CHECK_UPDATE_THREAD_COMPLETED, wxThreadEvent);

namespace app::dialog
{
class CheckForUpdateDialog;

class CheckForUpdateThread : public wxThread
{
public:
    CheckForUpdateThread() = delete;
    explicit CheckForUpdateThread(CheckForUpdateDialog* handler);
    virtual ~CheckForUpdateThread();

protected:
    ExitCode Entry() override;

private:
    CheckForUpdateDialog* pHandler;
};

class CheckForUpdateDialog : public wxDialog
{
public:
    CheckForUpdateDialog() = delete;
    explicit CheckForUpdateDialog(wxWindow* parent, const wxString& name = wxT("checkforupdate"));
    virtual ~CheckForUpdateDialog() = default;

    void LaunchModal();

protected:
    CheckForUpdateThread* pThread;
    wxCriticalSection mCriticalSection;

private:
    wxDECLARE_EVENT_TABLE();

    void Create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& position,
        const wxSize& size,
        long style,
        const wxString& name);

    void CreateControls();

    void StartThread();

    void ThreadCleanupProcedure();

    void OnHyperLinkClicked(wxHyperlinkEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnThreadCompletion(wxThreadEvent& event);
    void OnClose(wxCloseEvent& event);

    friend class CheckForUpdateThread;

    wxButton* pOkButton;
    wxStaticText* pCheckingUpdateText;
    wxStaticText* pNewReleaseAvailable;
    wxHyperlinkCtrl* pNewReleaseLink;
    wxGauge* pGaugeCtrl;

    enum { IDC_CHECK_NEW_RELEASE = wxID_HIGHEST + 1, IDC_NEW_RELEASE_AVAILABLE, IDC_NEW_RELEASE_LINK, IDC_GAUGE };
};
} // namespace app::dialog
