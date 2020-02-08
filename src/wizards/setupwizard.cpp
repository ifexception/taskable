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

#include "setupwizard.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>

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
    , pDatabase(nullptr)
{
    pPage1 = new WelcomePage(this);
    auto page2 = new AddEmployerAndClientPage(this);
    auto page3 = new AddProjectPage(this);

    wxWizardPageSimple::Chain(pPage1, page2);
    wxWizardPageSimple::Chain(page2, page3);
}

SetupWizard::~SetupWizard()
{
    Cleanup();
}

bool SetupWizard::Run()
{
    auto wizardSuccess = wxWizard::RunWizard(pPage1);
    if (wizardSuccess) {
        CreateDatabaseFile();

        InitializeSqliteConnection();
        bool success = SetUpTables();
        if (!success) {
            Cleanup();
            DeleteDatabaseFile();
            return false;
        }

        success = SetUpEntities();
        if (!success) {
            Cleanup();
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
    if (!wxDirExists(common::GetDatabasePath())) {
        if (wxMkdir(common::GetDatabasePath())) {
            wxFile file;
            file.Create(common::GetDatabaseFilePath());
            file.Close();
        }
    } else {

    }
}

void SetupWizard::InitializeSqliteConnection()
{
    auto config = sqlite::sqlite_config{ sqlite::OpenFlags::READWRITE, nullptr, sqlite::Encoding::UTF8 };
    pDatabase = new sqlite::database(common::GetDatabaseFilePath().ToStdString(), config);

    svc::DatabaseConnection::Get().SetHandle(pDatabase);
}

void SetupWizard::Cleanup()
{
    delete pDatabase;
    svc::DatabaseConnection::Get().UnsetHandle();
}

void SetupWizard::DeleteDatabaseFile()
{
    if (wxFileExists(common::GetDatabaseFilePath())) {
        wxRemoveFile(common::GetDatabaseFilePath());
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
                                      "on your computer.");
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
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto employerSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(employerSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    wxString employerDescriptiveText = wxT("An employer is the company that employs you\n"
                                           "(this can be a company or self-employment)");
    auto employerTextCtrl = new wxStaticText(this, wxID_ANY, employerDescriptiveText);
    employerSizer->Add(employerTextCtrl, wxSizerFlags().Border(wxALL, 5));

    auto employerHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(1, 1), wxLI_HORIZONTAL);
    employerSizer->Add(employerHorizontalLine, wxSizerFlags().Expand());

    auto employerHSizer = new wxBoxSizer(wxHORIZONTAL);
    auto employerText = new wxStaticText(this, wxID_ANY, wxT("Employer:"));
    employerHSizer->Add(employerText, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    pEmployerCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pEmployerCtrl->SetHint(wxT("Employer name"));
    pEmployerCtrl->SetToolTip(wxT("Specify a descriptive name for an employer"));
    employerHSizer->Add(pEmployerCtrl, wxSizerFlags().Border(wxALL, 5));
    employerSizer->Add(employerHSizer, 0);

    mainSizer->AddSpacer(16);

    auto horizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    mainSizer->Add(horizontalLine, wxSizerFlags().Expand());

    mainSizer->AddSpacer(16);

    auto clientSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(clientSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    wxString clientDescriptiveText = wxT("A client is a beneficiary of an employer\n"
                                         "(utilizes a person's/companys services)");
    auto clientTextCtrl = new wxStaticText(this, wxID_ANY, clientDescriptiveText);
    clientSizer->Add(clientTextCtrl, wxSizerFlags().Border(wxALL, 5));

    auto clientHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(1, 1), wxLI_HORIZONTAL);
    clientSizer->Add(clientHorizontalLine, wxSizerFlags().Expand());

    auto clientHSizer = new wxBoxSizer(wxHORIZONTAL);
    auto clientText = new wxStaticText(this, wxID_ANY, wxT("Client:*"));
    clientHSizer->Add(clientText, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    pClientCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pClientCtrl->SetHint(wxT("Client name (optional)"));
    pClientCtrl->SetToolTip(wxT("Specify a client name to associate to the employer above"));
    clientHSizer->Add(pClientCtrl, wxSizerFlags().Border(wxALL, 5));

    clientSizer->Add(clientHSizer, 0);

    SetSizerAndFit(mainSizer);
}

bool AddEmployerAndClientPage::TransferDataFromWindow()
{
    const wxString employer = pEmployerCtrl->GetValue().Trim();
    if (employer.empty()) {
        wxMessageBox(wxT("An employer is required"), common::GetProgramName(), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxString client = pClientCtrl->GetValue().Trim();

    pParent->SetEmployer(employer);
    pParent->SetClient(client);

    return true;
}

void AddEmployerAndClientPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(
        wxT("Are you sure want to cancel the setup and exit?"), common::GetProgramName(), wxICON_QUESTION | wxYES_NO);
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
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto projectSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(projectSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto projectInfoText = wxT("A business undertaking as a individual or\n"
                               "team to achieve a business goal");
    auto projectInfoTextCtrl = new wxStaticText(this, wxID_ANY, projectInfoText);
    projectSizer->Add(projectInfoTextCtrl, wxSizerFlags().Border(wxALL, 5));

    auto projectHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(2, 2), wxLI_HORIZONTAL);
    projectSizer->Add(projectHorizontalLine, wxSizerFlags().Expand());

    auto projectNameHSizer = new wxBoxSizer(wxHORIZONTAL);
    projectSizer->Add(projectNameHSizer, 1);

    auto projectLabel = new wxStaticText(this, wxID_ANY, wxT("Project:"));
    projectNameHSizer->Add(projectLabel, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    projectNameHSizer->AddStretchSpacer(1);

    pNameCtrl = new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pNameCtrl->SetHint(wxT("Project name"));
    pNameCtrl->SetToolTip(wxT("Specify a project name to associate with said employer and/or client"));
    projectNameHSizer->Add(pNameCtrl, wxSizerFlags().Border(wxALL, 5));

    projectSizer->AddSpacer(8);

    wxString displayNameInfo = wxT("A shortened version of a project name to\n"
                                   "aid with legibility and display.");
    auto displayNameInfoTextCtrl = new wxStaticText(this, wxID_ANY, displayNameInfo);
    projectSizer->Add(displayNameInfoTextCtrl, wxSizerFlags().Border(wxALL, 5));

    auto displayNameHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(2, 2), wxLI_HORIZONTAL);
    projectSizer->Add(displayNameHorizontalLine, wxSizerFlags().Expand());

    auto displayNameHSizer = new wxBoxSizer(wxHORIZONTAL);
    projectSizer->Add(displayNameHSizer, 1);

    auto displayNameLabel = new wxStaticText(this, wxID_ANY, wxT("Display Name:"));
    displayNameHSizer->Add(displayNameLabel, wxSizerFlags().Border(wxALL, 5));

    pDisplayNameCtrl =
        new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pDisplayNameCtrl->SetHint(wxT("Display name"));
    pDisplayNameCtrl->SetToolTip(wxT("Specify a shortened version of the project name"));
    displayNameHSizer->Add(pDisplayNameCtrl, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    SetSizerAndFit(mainSizer);
}

bool AddProjectPage::TransferDataFromWindow()
{
    const wxString projectName = pNameCtrl->GetValue().Trim();
    if (projectName.empty()) {
        wxMessageBox(wxT("A project name is required"), wxT("Taskable"), wxOK | wxICON_ERROR, this);
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
    auto userResponse = wxMessageBox(
        wxT("Are you sure want to cancel the setup and exit?"), wxT("Taskable Wizard"), wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}
} // namespace app::wizard
