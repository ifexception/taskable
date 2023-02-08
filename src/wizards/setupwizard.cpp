// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#include "setupwizard.h"

#include <sqlite_modern_cpp/errors.h>

#include <wx/colour.h>
#include <wx/file.h>
#include <wx/statline.h>
#include <wx/valnum.h>

#include "../common/common.h"
#include "../common/util.h"

#include "../models/employermodel.h"
#include "../models/clientmodel.h"
#include "../models/projectmodel.h"

#include "../../res/setupwizard.xpm"

namespace app::wizard
{
SetupWizard::SetupWizard(wxFrame* frame,
    std::shared_ptr<spdlog::logger> logger)
    : wxWizard(frame, wxID_ANY, wxT("Setup Wizard"), wxBitmap(setupwizard), wxDefaultPosition, wxDEFAULT_DIALOG_STYLE)
    , pLogger(logger)
    , pCompositor(std::make_shared<EntityCompositor>(logger))
    , pPage1(nullptr)
{
    SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);

    pPage1 = new WelcomePage(this);
    auto page2 = new AddEmployerPage(this, pCompositor);
    auto page3 = new AddClientPage(this, pCompositor);
    auto page4 = new AddProjectPage(this, pLogger, pCompositor);

    wxWizardPageSimple::Chain(pPage1, page2);
    wxWizardPageSimple::Chain(page2, page3);
    wxWizardPageSimple::Chain(page3, page4);

    // Allow the wizard to size itself around the pages
    GetPageAreaSizer()->Add(pPage1);
}

bool SetupWizard::Run()
{
    bool wizardSuccess = wxWizard::RunWizard(pPage1);

    Destroy();
    return wizardSuccess;
}

WelcomePage::WelcomePage(wxWizard* parent)
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
wxBEGIN_EVENT_TABLE(AddEmployerPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddEmployerPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddEmployerPage::AddEmployerPage(wxWizard* parent, std::shared_ptr<EntityCompositor> compositor)
    : wxWizardPageSimple(parent)
    , pParent(parent)
    , pCompositor(compositor)
    , pEmployerTextCtrl(nullptr)
// clang-format on
{
    CreateControls();
}

bool AddEmployerPage::TransferDataFromWindow()
{
    const wxString employer = pEmployerTextCtrl->GetValue().Trim();
    if (employer.empty()) {
        wxMessageBox(wxT("An employer is required"), common::GetProgramName(), wxOK | wxICON_ERROR, this);
        return false;
    }

    if (!pCompositor->ComposeEmployerEntity(std::make_unique<model::EmployerModel>(employer))) {
        wxMessageBox(wxT("An error occured during the Employer save operation."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_ERROR);
        return false;
    }

    return true;
}

void AddEmployerPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup wizard and exit?"),
        common::GetProgramName(),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

void AddEmployerPage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    /* Wizard page heading */
    wxString addEmployerString = wxT("Employer Setup");
    auto addEmployerLabel = new wxStaticText(this, wxID_ANY, addEmployerString);
    auto addEmployerLabelFont = addEmployerLabel->GetFont();
    addEmployerLabelFont.MakeBold();
    addEmployerLabelFont.SetPointSize(13);
    addEmployerLabel->SetFont(addEmployerLabelFont);
    mainSizer->Add(addEmployerLabel, wxSizerFlags().Border(wxALL, 5));

    mainSizer->AddSpacer(2);

    /* Wizard page subtitle */
    wxString employerOptionalString = wxT("required");
    auto employerOptionalLabel = new wxStaticText(this, wxID_ANY, employerOptionalString);
    auto employerOptionalLabelFont = employerOptionalLabel->GetFont();
    employerOptionalLabelFont.MakeBold();
    employerOptionalLabelFont.SetPointSize(8);
    employerOptionalLabel->SetFont(employerOptionalLabelFont);
    mainSizer->Add(employerOptionalLabel, wxSizerFlags().Border(wxALL, 5));

    mainSizer->AddSpacer(2);

    auto employerHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    mainSizer->Add(employerHorizontalLine, wxSizerFlags().Expand());

    mainSizer->AddSpacer(2);

    /* Information Box */
    auto informationBox = new wxStaticBox(this, wxID_ANY, wxT("Information"));
    auto informationBoxSizer = new wxStaticBoxSizer(informationBox, wxVERTICAL);

    /* Employer description text */
    wxString employerDescriptiveText = wxT("A person or organization or business entity\n"
                                           "that employs you.\n"
                                           "This includes self-employment.");
    auto employerTextCtrl = new wxStaticText(informationBox, wxID_ANY, employerDescriptiveText);
    informationBoxSizer->Add(employerTextCtrl, wxSizerFlags().Border(wxALL, 5));
    mainSizer->Add(informationBoxSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto employerSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(employerSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    employerSizer->AddSpacer(8);

    auto employerHSizer = new wxBoxSizer(wxHORIZONTAL);
    auto employerText = new wxStaticText(this, wxID_ANY, wxT("Employer:"));
    employerHSizer->Add(employerText, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    wxTextValidator employerInputValidator(wxFILTER_ALPHANUMERIC | wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString allowedChars;
    allowedChars.Add(wxT(" "));
    allowedChars.Add(wxT("."));
    allowedChars.Add(wxT("'"));
    allowedChars.Add(wxT("("));
    allowedChars.Add(wxT(")"));
    allowedChars.Add(wxT("-"));
    employerInputValidator.SetIncludes(allowedChars);

    pEmployerTextCtrl = new wxTextCtrl(
        this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, employerInputValidator);
    pEmployerTextCtrl->SetHint(wxT("Employer name"));
    pEmployerTextCtrl->SetToolTip(wxT("Specify a descriptive name for an employer"));
    employerHSizer->Add(pEmployerTextCtrl, wxSizerFlags().Border(wxALL, 5));
    employerSizer->Add(employerHSizer, 0);

    mainSizer->AddSpacer(16);

    SetSizerAndFit(mainSizer);
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddClientPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddClientPage::OnWizardCancel)
wxEND_EVENT_TABLE()

AddClientPage::AddClientPage(wxWizard* parent, std::shared_ptr<EntityCompositor> compositor)
    : wxWizardPageSimple(parent)
    , pParent(parent)
    , pCompositor(compositor)
    , pClientTextCtrl(nullptr)
// clang-format on
{
    CreateControls();
}

bool AddClientPage::TransferDataFromWindow()
{
    const wxString clientName = pClientTextCtrl->GetValue().Trim();
    if (clientName.empty()) {
        return true;
    }

    if (!clientName.empty() && clientName.length() < 2) {
        wxMessageBox(wxT("An client name is required to be at least 2 characters long"),
            common::GetProgramName(),
            wxOK | wxICON_ERROR,
            this);
        return false;
    }

    std::unique_ptr<model::ClientModel> clientModel = std::make_unique<model::ClientModel>();
    clientModel->SetName(clientName);
    if (!pCompositor->ComposeClientEntity(std::move(clientModel))) {
        wxMessageBox(wxT("An error occured during the Client save operation."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_ERROR);
        return false;
    }

    return true;
}

void AddClientPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup wizard and exit?"),
        common::GetProgramName(),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

void AddClientPage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    /* Wizard page heading */
    wxString addClientString = wxT("Client Setup");
    auto addClientLabel = new wxStaticText(this, wxID_ANY, addClientString);
    auto addClientLabelFont = addClientLabel->GetFont();
    addClientLabelFont.MakeBold();
    addClientLabelFont.SetPointSize(13);
    addClientLabel->SetFont(addClientLabelFont);
    mainSizer->Add(addClientLabel, wxSizerFlags().Border(wxALL, 5));

    mainSizer->AddSpacer(2);

    /* Wizard page subtitle */
    wxColourDatabase colorDatabase;
    wxString clientOptionalString = wxT("optional");
    auto clientOptionalLabel = new wxStaticText(this, wxID_ANY, clientOptionalString);
    auto clientOptionalLabelFont = clientOptionalLabel->GetFont();
    clientOptionalLabelFont.MakeItalic();
    clientOptionalLabelFont.SetPointSize(8);
    clientOptionalLabel->SetFont(clientOptionalLabelFont);
    clientOptionalLabel->SetForegroundColour(colorDatabase.Find(wxT("DARK GREY")));
    mainSizer->Add(clientOptionalLabel, wxSizerFlags().Border(wxALL, 5));

    mainSizer->AddSpacer(2);

    auto clientHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    mainSizer->Add(clientHorizontalLine, wxSizerFlags().Expand());

    mainSizer->AddSpacer(2);

    /* Information Box */
    auto informationBox = new wxStaticBox(this, wxID_ANY, wxT("Information"));
    auto informationBoxSizer = new wxStaticBoxSizer(informationBox, wxVERTICAL);

    /* Client description text */
    wxString clientDescriptiveText = wxT("A client is a beneficiary of an employer that\n"
                                         "utilizes (buys or pays) the services/goods\n"
                                         "provided by a person or a company.");
    auto clientTextCtrl = new wxStaticText(informationBox, wxID_ANY, clientDescriptiveText);
    informationBoxSizer->Add(clientTextCtrl, wxSizerFlags().Border(wxALL, 5));
    mainSizer->Add(informationBoxSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto clientSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(clientSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    clientSizer->AddSpacer(8);

    auto clientHSizer = new wxBoxSizer(wxHORIZONTAL);
    auto clientText = new wxStaticText(this, wxID_ANY, wxT("Client:"));
    clientHSizer->Add(clientText, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    wxTextValidator clientInputValidator(wxFILTER_ALPHANUMERIC | wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString allowedChars;
    allowedChars.Add(wxT(" "));
    allowedChars.Add(wxT("."));
    allowedChars.Add(wxT("'"));
    allowedChars.Add(wxT("("));
    allowedChars.Add(wxT(")"));
    allowedChars.Add(wxT("-"));
    clientInputValidator.SetIncludes(allowedChars);

    pClientTextCtrl = new wxTextCtrl(
        this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, clientInputValidator);
    pClientTextCtrl->SetHint(wxT("Client name"));
    pClientTextCtrl->SetToolTip(wxT("Specify a descriptive name for a client"));
    clientHSizer->Add(pClientTextCtrl, wxSizerFlags().Border(wxALL, 5));
    clientSizer->Add(clientHSizer, 0);

    mainSizer->AddSpacer(16);

    SetSizerAndFit(mainSizer);
}

// clang-format off
wxBEGIN_EVENT_TABLE(AddProjectPage, wxWizardPageSimple)
    EVT_WIZARD_CANCEL(wxID_ANY, AddProjectPage::OnWizardCancel)
    EVT_TEXT(IDC_NAME, AddProjectPage::OnNameChange)
wxEND_EVENT_TABLE()

AddProjectPage::AddProjectPage(SetupWizard* parent,
    std::shared_ptr<spdlog::logger> logger,
    std::shared_ptr<EntityCompositor> compositor)
    : wxWizardPageSimple(parent)
    , pLogger(logger)
    , pCompositor(compositor)
    , pParent(parent)
    , pNameTextCtrl(nullptr)
    , pDisplayNameTextCtrl(nullptr)
// clang-format on
{
    CreateControls();
}

bool AddProjectPage::TransferDataFromWindow()
{
    const wxString projectName = pNameTextCtrl->GetValue().Trim();
    if (projectName.empty()) {
        wxMessageBox(wxT("A project name is required"), wxT("Taskable"), wxOK | wxICON_ERROR, this);
        return false;
    }

    const wxString displayName = pDisplayNameTextCtrl->GetValue().Trim();
    if (displayName.empty()) {
        wxMessageBox(wxT("A display name is required"), wxT("Taskable"), wxOK | wxICON_ERROR, this);
        return false;
    }

    std::unique_ptr<model::ProjectModel> project = std::make_unique<model::ProjectModel>();
    project->SetName(projectName);
    project->SetDisplayName(projectName);
    if (!pCompositor->ComposeProjectEntity(std::move(project))) {
        wxMessageBox(wxT("An error occured during the Project save operation."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_ERROR);
        return false;
    }

    return true;
}

void AddProjectPage::OnWizardCancel(wxWizardEvent& event)
{
    auto userResponse = wxMessageBox(wxT("Are you sure want to cancel the setup wizard and exit?"),
        wxT("Taskable Wizard"),
        wxICON_QUESTION | wxYES_NO);
    if (userResponse == wxNO) {
        event.Veto();
    }
}

void AddProjectPage::OnNameChange(wxCommandEvent& event)
{
    wxString name = pNameTextCtrl->GetValue();
    pDisplayNameTextCtrl->ChangeValue(name);
}

void AddProjectPage::CreateControls()
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    /* Wizard page heading */
    wxString addProjectString = wxT("Project Setup");
    auto addProjectLabel = new wxStaticText(this, wxID_ANY, addProjectString);
    auto addProjectLabelFont = addProjectLabel->GetFont();
    addProjectLabelFont.MakeBold();
    addProjectLabelFont.SetPointSize(13);
    addProjectLabel->SetFont(addProjectLabelFont);
    mainSizer->Add(addProjectLabel, wxSizerFlags().Border(wxALL, 5));

    mainSizer->AddSpacer(2);

    /* Wizard page subtitle */
    wxString projectRequiredString = wxT("required");
    auto projectRequiredLabel = new wxStaticText(this, wxID_ANY, projectRequiredString);
    auto projectRequiredLabelFont = projectRequiredLabel->GetFont();
    projectRequiredLabelFont.MakeBold();
    projectRequiredLabelFont.SetPointSize(8);
    projectRequiredLabel->SetFont(projectRequiredLabelFont);
    mainSizer->Add(projectRequiredLabel, wxSizerFlags().Border(wxALL, 5));

    mainSizer->AddSpacer(2);

    auto projectHorizontalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3, 3), wxLI_HORIZONTAL);
    mainSizer->Add(projectHorizontalLine, wxSizerFlags().Expand());

    mainSizer->AddSpacer(2);

    /* Information Box */
    auto informationBox = new wxStaticBox(this, wxID_ANY, wxT("Information"));
    auto informationBoxSizer = new wxStaticBoxSizer(informationBox, wxVERTICAL);

    /* Project description text */
    wxString projectDescriptiveText = wxT("A project is a piece of work that is planned\n"
                                          "within a business environment to achieve\n"
                                          "business objectives that align to a\n"
                                          "company's business strategy.");
    auto projectTextCtrl = new wxStaticText(informationBox, wxID_ANY, projectDescriptiveText);
    informationBoxSizer->Add(projectTextCtrl, wxSizerFlags().Border(wxALL, 5));
    mainSizer->Add(informationBoxSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto projectSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(projectSizer, wxSizerFlags().Border(wxALL, 5).Expand());

    auto projectFlexGridSizer = new wxFlexGridSizer(2, 1, 1);
    projectSizer->Add(projectFlexGridSizer, 1);

    /* Project Name Text Control */
    auto projectLabel = new wxStaticText(this, wxID_ANY, wxT("Project:"));
    projectFlexGridSizer->Add(projectLabel, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    pNameTextCtrl = new wxTextCtrl(this, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pNameTextCtrl->SetHint(wxT("Project name"));
    pNameTextCtrl->SetToolTip(wxT("Specify a project name to associate with said employer and/or client"));
    projectFlexGridSizer->Add(pNameTextCtrl, wxSizerFlags().Border(wxALL, 5));

    /* Project Display Name Text Control */
    auto displayNameLabel = new wxStaticText(this, wxID_ANY, wxT("Display Name:"));
    projectFlexGridSizer->Add(displayNameLabel, wxSizerFlags().Border(wxALL, 5));

    pDisplayNameTextCtrl =
        new wxTextCtrl(this, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pDisplayNameTextCtrl->SetHint(wxT("Display name"));
    pDisplayNameTextCtrl->SetToolTip(wxT("Specify a shortened version of the project name"));
    projectFlexGridSizer->Add(pDisplayNameTextCtrl, wxSizerFlags().Border(wxALL, 5).CenterVertical());

    SetSizerAndFit(mainSizer);
}
} // namespace app::wizard
