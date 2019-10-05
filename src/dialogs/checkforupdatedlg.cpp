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

#include "checkforupdatedlg.h"

#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <wx/statline.h>

#include "../common/common.h"
#include "../common/util.h"

using json = nlohmann::json;

wxDEFINE_EVENT(CHECK_UPDATE_THREAD_COMPLETED, wxThreadEvent);

namespace app::dialog
{
CheckForUpdateThread::CheckForUpdateThread(CheckForUpdateDialog* handler)
    : wxThread(wxTHREAD_DETACHED)
    , pHandler(handler)
{
}

CheckForUpdateThread::~CheckForUpdateThread()
{
    wxCriticalSectionLocker enter(pHandler->mCriticalSection);
    pHandler->pThread = nullptr;
}

wxThread::ExitCode CheckForUpdateThread::Entry()
{
    std::string eventString = "";

    if (TestDestroy()) {
        return nullptr;
    }

    std::string githubUrl = "https://api.github.com";
    std::string releasesApi = "/repos/ifexception/wx-tasks-tracker/releases/latest";
    std::string url = githubUrl + releasesApi;

    auto response = cpr::Get(cpr::Url{ url }, cpr::Header{ { "Accept", "application/vnd.github.v3+json" } });

    if (response.status_code == 200) {
        auto jsonText = json::parse(response.text);
        auto name = jsonText["name"].get<std::string>();
        auto description = jsonText["description"].get<std::string>();
        auto htmlUrl = jsonText["html_url"].get<std::string>();

        auto currentVersion = "v" + common::GetVersion();
        if (name != currentVersion) {
            eventString = name + "," + description + "," + htmlUrl;
        }
    }

    auto event = new wxThreadEvent(CHECK_UPDATE_THREAD_COMPLETED);
    event->SetString(eventString);
    wxQueueEvent(pHandler, event);

    return (wxThread::ExitCode) 0;
}

// clang-format off
wxBEGIN_EVENT_TABLE(CheckForUpdateDialog, wxDialog)
    EVT_CLOSE(CheckForUpdateDialog::OnClose)
    EVT_BUTTON(wxID_OK, CheckForUpdateDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, CheckForUpdateDialog::OnCancel)
    EVT_HYPERLINK(CheckForUpdateDialog::IDC_NEW_RELEASE_LINK, CheckForUpdateDialog::OnHyperLinkClicked)
wxEND_EVENT_TABLE()

CheckForUpdateDialog::CheckForUpdateDialog(wxWindow* parent, const wxString& name)
// clang-format on
{
    Create(parent,
        wxID_ANY,
        wxT("Check for Update"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);

    Bind(CHECK_UPDATE_THREAD_COMPLETED, &CheckForUpdateDialog::OnThreadCompletion, this);
}

void CheckForUpdateDialog::LaunchModal()
{
    pGaugeCtrl->Pulse();
    StartThread();
    wxDialog::ShowModal();
}

void CheckForUpdateDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    bool success = wxDialog::Create(parent, windowId, title, position, size, style, name);
    if (success) {
        CreateControls();

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }
}

void CheckForUpdateDialog::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto mainPanel = new wxPanel(this, wxID_STATIC);
    mainPanel->SetSizer(sizer);
    mainSizer->Add(mainPanel, common::sizers::ControlDefault);

    /* Check for new release Text Control */
    pCheckingUpdateText = new wxStaticText(mainPanel, IDC_CHECK_NEW_RELEASE, wxT("Checking for new release..."));
    sizer->Add(pCheckingUpdateText, common::sizers::ControlDefault);

    /* New Release Available Text Control */
    pNewReleaseAvailable = new wxStaticText(mainPanel, IDC_NEW_RELEASE_AVAILABLE, wxT(""));
    sizer->Add(pNewReleaseAvailable, common::sizers::ControlDefault);

    /* New Release Hyperlink Control */
    pNewReleaseLink = new wxHyperlinkCtrl(mainPanel,
        IDC_NEW_RELEASE_LINK,
        wxT("New Release"),
        wxT("https://www.github.com/ifexception/wx-tasks-tracker/releases"),
        wxDefaultPosition,
        wxDefaultSize,
        wxHL_DEFAULT_STYLE);
    pNewReleaseLink->Hide();
    sizer->Add(pNewReleaseLink, common::sizers::ControlDefault);

    /* Gauge Control */
    pGaugeCtrl = new wxGauge(mainPanel, IDC_GAUGE, 100);
    sizer->Add(pGaugeCtrl, common::sizers::ControlExpand);

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("&OK"));
    pOkButton->Disable();

    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));
    cancelButton->SetFocus();

    buttonPanelSizer->Add(pOkButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void CheckForUpdateDialog::StartThread()
{
    pThread = new CheckForUpdateThread(this);
    auto ret = pThread->Run();
    if (ret != wxTHREAD_NO_ERROR) {
        delete pThread;
        pThread = nullptr;
    }
}

void CheckForUpdateDialog::ThreadCleanupProcedure()
{
    {
        wxCriticalSectionLocker enter(mCriticalSection);
        if (pThread) {
            auto ret = pThread->Delete();
            if (ret != wxTHREAD_NO_ERROR) {
                wxLogError("Cannot delete thread!");
            }
        }
    }

    while (1) {
        {
            wxCriticalSectionLocker enter(mCriticalSection);
            if (!pThread) {
                break;
            }
        }
        wxThread::This()->Sleep(1);
    }

    Destroy();
}

void CheckForUpdateDialog::OnHyperLinkClicked(wxHyperlinkEvent& event)
{
    wxLaunchDefaultBrowser(event.GetURL());
}

void CheckForUpdateDialog::OnOk(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void CheckForUpdateDialog::OnCancel(wxCommandEvent& event)
{
    ThreadCleanupProcedure();
}

void CheckForUpdateDialog::OnThreadCompletion(wxThreadEvent& event)
{
    pOkButton->Enable();
    pGaugeCtrl->SetValue(100);
    auto eventString = event.GetString();
    if (!eventString.empty()) {
        auto releaseInfo = util::lib::split(eventString.ToStdString(), ',');
        auto version = releaseInfo[0];
        auto description = releaseInfo[1];
        auto downloadLink = releaseInfo[2];

        auto text = wxString::Format(
            wxT("New release available!\nCurrent version: v%s\nNew version: %s\nDescription: %s\nPlease click the "
                "link below to download the new version"),
            common::GetVersion(),
            version,
            description);
        pNewReleaseAvailable->SetLabel(text);
        pNewReleaseLink->SetLabel(downloadLink);
        pNewReleaseLink->Show();
    } else {
        auto text = wxT("No new release available.");
        pNewReleaseAvailable->SetLabel(text);
    }
}

void CheckForUpdateDialog::OnClose(wxCloseEvent& event)
{
    ThreadCleanupProcedure();
}

} // namespace app::dialog
