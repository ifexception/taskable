//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2019><Szymon Welgus>
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
//
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "setupwizard.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/wx.h>
#include <wx/file.h>

#include "../common/common.h"
#include "../../res/setupwizard.xpm"

namespace app::wizard
{
SetupWizard::SetupWizard(wxFrame* frame, std::shared_ptr<spdlog::logger> logger)
    : wxWizard(frame, wxID_ANY, wxT("Setup Wizard"), wxBitmap(setupwizard), wxDefaultPosition, wxDEFAULT_DIALOG_STYLE)
    , pLogger(logger)
    , pFrame(frame)
    , pPage1(nullptr)
    , mEmployer(wxGetEmptyString())
    , mClient(wxGetEmptyString())
    , mProject(wxGetEmptyString())
{
    pPage1 = new WelcomePage(this);
    auto page2 = new AddEmployerAndClientPage(this);
    auto page3 = new AddProjectPage(this);

    wxWizardPageSimple::Chain(pPage1, page2);
    wxWizardPageSimple::Chain(page2, page3);
}

bool SetupWizard::Run()
{
    auto wizardSuccess = wxWizard::RunWizard(pPage1);
    if (wizardSuccess) {
        CreateDatabaseFile();
        bool success = SetUpTables();
        if (!success) {
            DeleteDatabaseFile();
            return false;
        }

        success = SetUpEntities();
        if (!success) {
            DeleteDatabaseFile();
            return false;
        }
    }

    Destroy();
    return wizardSuccess;
}

wxString SetupWizard::GetEmployer() const
{
    return mEmployer;
}

void SetupWizard::SetEmployer(const wxString& employer)
{
    mEmployer = employer;
}

wxString SetupWizard::GetClient() const
{
    return mClient;
}

void SetupWizard::SetClient(const wxString& client)
{
    mClient = client;
}

wxString SetupWizard::GetProject() const
{
    return mProject;
}

void SetupWizard::SetProject(const wxString& project)
{
    mProject = project;
}

void SetupWizard::SetProjectDisplayName(const wxString& displayName)
{
    mDisplayName = displayName;
}

void SetupWizard::CreateDatabaseFile()
{
    const wxString& databaseFilename = common::GetDbFileName();
    wxFile file;
    file.Create(databaseFilename);
}

void SetupWizard::DeleteDatabaseFile()
{
    const wxString& databaseFilename = common::GetDbFileName();
    if (wxFileExists(databaseFilename)) {
        wxRemoveFile(databaseFilename);
    }
}

bool SetupWizard::SetUpTables()
{
    SetupTables tables(pLogger);
    return tables.CreateTables();
}

bool SetupWizard::SetUpEntities()
{
    SetupEntities entities(pLogger);
    return entities.CreateEntities(mEmployer, mClient, mProject, mDisplayName);
}

WelcomePage::WelcomePage(SetupWizard* parent)
    : wxWizardPageSimple(parent)
{
    CreateControls();
}

void WelcomePage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    /* Welcome message Static Text Control */
    wxString welcomeMessage = wxT("Welcome to the Taskable\n"
                                  "Setup Wizard");
    auto welcomeText = new wxStaticText(this, wxID_ANY, welcomeMessage);
    auto welcomeTextFont = welcomeText->GetFont();
    welcomeTextFont.MakeBold();
    welcomeTextFont.SetPointSize(16);
    welcomeText->SetFont(welcomeTextFont);
    mainSizer->Add(welcomeText, wxSizerFlags().Border(wxALL, 5));

    /* Wizard introduction Static Text Control */
    wxString introWizardMessage = wxT("This wizard will help you get Taskable setup\n"
                                      "on your computer.\n");
    auto introText = new wxStaticText(this, wxID_ANY, introWizardMessage);
    mainSizer->Add(introText, wxSizerFlags().Border(wxALL, 5));

    /* Continue next Static Text Control */
    wxString continueNextMessage = wxT("\n\n\nTo continue, click Next.");
    auto continueNextText = new wxStaticText(this, wxID_ANY, continueNextMessage);
    mainSizer->Add(continueNextText, wxSizerFlags().Border(wxALL, 5));

    SetSizerAndFit(mainSizer);
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddEmployerAndClientPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddEmployerAndClientPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddEmployerAndClientPage::AddEmployerAndClientPage(SetupWizard* parent)
    : wxWizardPageSimple(parent)
    , pParent(parent)
// clang-format on
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    auto employerText = new wxStaticText(this, wxID_ANY, wxT("Employer:"));
    sizer->Add(employerText, 0, wxALL, 5);

    pEmployerCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    sizer->Add(pEmployerCtrl, 0, wxALL, 5);

    wxString employerHelpMessage =
        wxT("Specify a descriptive name for an employer.\n"
            "An employer is whoever you work for and under who all data will be grouped under");
    auto employerHelpText = new wxStaticText(this, wxID_ANY, employerHelpMessage);
    sizer->Add(employerHelpText, 0, wxALL, 5);

    auto clientText = new wxStaticText(this, wxID_ANY, wxT("Client:*"));
    sizer->Add(clientText, 0, wxALL, 5);

    pClientCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    sizer->Add(pClientCtrl, 0, wxALL, 5);

    wxString clientHelpMessage =
        wxT("Specify a descriptive name for a client.\n"
            "If your employer has multiple clients and you work with them then you can add a client\n"
            "A client is, however, optional and can be safely skipped if you do not deal with clients");
    auto clientHelpText = new wxStaticText(this, wxID_ANY, clientHelpMessage);
    sizer->Add(clientHelpText, 0, wxALL, 5);

    SetSizer(sizer);
    sizer->Fit(this);
}

bool AddEmployerAndClientPage::TransferDataFromWindow()
{
    const wxString employer = pEmployerCtrl->GetValue().Trim();
    if (employer.empty()) {
        wxMessageBox(wxT("An employer is required"), wxT("Taskable"), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxString client = pClientCtrl->GetValue().Trim();

    pParent->SetEmployer(employer);
    pParent->SetClient(client);

    return true;
}

void AddEmployerAndClientPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup and exit the wizard?"),
        wxT("Taskable Wizard"),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddProjectPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddProjectPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddProjectPage::AddProjectPage(SetupWizard* parent)
    : wxWizardPageSimple(parent)
    , pParent(parent)
// clang-format on
{
    auto sizer = new wxBoxSizer(wxVERTICAL);

    wxString infoMessage = wxGetEmptyString();
    if (!pParent->GetClient().empty()) {
        infoMessage = wxString::Format(
            "Add a project for employer: %s and client: %s", pParent->GetEmployer(), pParent->GetClient());
    } else {
        infoMessage = wxString::Format("Add a project for employer: %s", pParent->GetEmployer());
    }

    auto infoText = new wxStaticText(this, wxID_ANY, infoMessage);
    sizer->Add(infoText, 0, wxALL, 5);

    auto projectText = new wxStaticText(this, wxID_ANY, wxT("Project:"));
    sizer->Add(projectText, 0, wxALL, 5);

    pNameCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    sizer->Add(pNameCtrl, 0, wxALL, 5);

    wxString projectNameHelpMessage =
        wxT("Specify a descriptive project name.\n"
            "A project is an undertaking of a business for a client or for itself carried "
            "out individually or in a group to achieve a business goal");
    auto projectNameHelpText = new wxStaticText(this, wxID_ANY, projectNameHelpMessage);
    sizer->Add(projectNameHelpText, 0, wxALL, 5);

    auto displayNameText = new wxStaticText(this, wxID_ANY, wxT("Display Name:"));
    sizer->Add(displayNameText, 0, wxALL, 5);

    pDisplayNameCtrl =
        new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    sizer->Add(pDisplayNameCtrl, 0, wxALL, 5);

    wxString displayNameHelpMessage = wxT("Specify a shortened version of the project name.\n"
                                          "Similar to a project name, a display name is a shortened version of "
                                          "the project name to aid in legibility and display");
    auto displayNameHelpText = new wxStaticText(this, wxID_ANY, displayNameHelpMessage);
    sizer->Add(displayNameHelpText, 0, wxALL, 5);

    SetSizer(sizer);
    sizer->Fit(this);
}

bool AddProjectPage::TransferDataFromWindow()
{
    const wxString projectName = pNameCtrl->GetValue().Trim();
    if (projectName.empty()) {
        wxMessageBox(wxT("An project name is required"), wxT("Taskable"), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxString displayName = pDisplayNameCtrl->GetValue().Trim();
    if (displayName.empty()) {
        wxMessageBox(wxT("A display name is required"), wxT("Taskable"), wxOK | wxICON_ERROR, this);
        return false;
    }

    pParent->SetProject(projectName);
    pParent->SetProjectDisplayName(displayName);

    return true;
}

void AddProjectPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup and exit the wizard?"),
        wxT("Taskable Wizard"),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}
} // namespace app::wizard
