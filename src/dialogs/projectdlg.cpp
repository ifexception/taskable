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

#include "projectdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/statline.h>
#include <wx/valnum.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../models/employermodel.h"
#include "../models/clientmodel.h"

namespace app::dlg
{
const wxString& ProjectDialog::DateLabel = wxT("Created %s | Updated %s");

ProjectDialog::ProjectDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , pNameTextCtrl(nullptr)
    , pDisplayNameCtrl(nullptr)
    , pEmployerChoiceCtrl(nullptr)
    , pClientChoiceCtrl(nullptr)
    , pBillableCtrl(nullptr)
    , pRateChoiceCtrl(nullptr)
    , pRateTextCtrl(nullptr)
    , pCurrencyComboBoxCtrl(nullptr)
    , pHoursTextCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pIsDefaultCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , pProject(std::make_unique<model::ProjectModel>())
    , mProjectId(0)
    , bIsEdit(false)
{
    Create(parent,
        wxID_ANY,
        wxT("Add Project"),
        wxDefaultPosition,
        wxSize(420, 380),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

ProjectDialog::ProjectDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    bool isEdit,
    int projectId,
    const wxString& name)
    : pLogger(logger)
    , pNameTextCtrl(nullptr)
    , pDisplayNameCtrl(nullptr)
    , pEmployerChoiceCtrl(nullptr)
    , pClientChoiceCtrl(nullptr)
    , pBillableCtrl(nullptr)
    , pRateChoiceCtrl(nullptr)
    , pRateTextCtrl(nullptr)
    , pCurrencyComboBoxCtrl(nullptr)
    , pHoursTextCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pIsDefaultCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , pProject(std::make_unique<model::ProjectModel>(projectId))
    , mProjectId(projectId)
    , bIsEdit(isEdit)
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Project"),
        wxDefaultPosition,
        wxSize(420, 440),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
}

bool ProjectDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
    bool created = wxDialog::Create(parent, windowId, title, position, size, style, name);

    if (created) {
        CreateControls();
        ConfigureEventBindings();
        FillControls();

        if (bIsEdit) {
            DataToControls();
        }

        GetSizer()->Fit(this);
        GetSizer()->SetSizeHints(this);
        SetIcon(common::GetProgramIcon());
        Center();
    }

    return created;
}

void ProjectDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Project Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Project Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    mainSizer->Add(detailsBoxSizer, common::sizers::ControlExpand);

    auto projectDetailsPanel = new wxPanel(this, wxID_ANY);
    detailsBoxSizer->Add(projectDetailsPanel, common::sizers::ControlExpand);

    auto flexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    projectDetailsPanel->SetSizer(flexGridSizer);

    /* ---Controls--- */
    /* Project Name Control */
    auto projectName = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Name"));
    flexGridSizer->Add(projectName, common::sizers::ControlCenterVertical);

    wxTextValidator nameValidator(wxFILTER_ALPHANUMERIC | wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString allowedChars;
    allowedChars.Add(wxT(" ")); // space
    allowedChars.Add(wxT(":")); // colon
    allowedChars.Add(wxT("-")); // dash
    nameValidator.SetIncludes(allowedChars);

    pNameTextCtrl = new wxTextCtrl(projectDetailsPanel,
        IDC_NAME,
        wxGetEmptyString(),
        wxDefaultPosition,
        wxSize(150, -1),
        wxTE_LEFT,
        nameValidator);
    pNameTextCtrl->SetHint(wxT("Project name"));
    pNameTextCtrl->SetToolTip(wxT("Enter a name for the project"));
    flexGridSizer->Add(pNameTextCtrl, common::sizers::ControlDefault);

    /* Project Display Name Control */
    auto projectDisplayName = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Display Name"));
    flexGridSizer->Add(projectDisplayName, common::sizers::ControlCenterVertical);

    pDisplayNameCtrl = new wxTextCtrl(projectDetailsPanel,
        IDC_DISPLAYNAME,
        wxGetEmptyString(),
        wxDefaultPosition,
        wxSize(150, -1),
        wxTE_LEFT,
        nameValidator);
    pDisplayNameCtrl->SetHint(wxT("Display name"));
    pDisplayNameCtrl->SetToolTip(wxT("Enter a shortened, convenient display name for the project"));
    flexGridSizer->Add(pDisplayNameCtrl, common::sizers::ControlDefault);

    /* Employer Choice Ctrl */
    auto employerText = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Employer"));
    flexGridSizer->Add(employerText, common::sizers::ControlCenterVertical);

    pEmployerChoiceCtrl = new wxChoice(projectDetailsPanel, IDC_EMPLOYERCHOICE, wxDefaultPosition, wxSize(150, -1));
    pEmployerChoiceCtrl->AppendString(wxT("Select a employer"));
    pEmployerChoiceCtrl->SetSelection(0);
    pEmployerChoiceCtrl->SetToolTip(wxT("Select a employer to associate the project with"));
    flexGridSizer->Add(pEmployerChoiceCtrl, common::sizers::ControlDefault);

    /* Client Choice Ctrl */
    auto clientText = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Client"));
    flexGridSizer->Add(clientText, common::sizers::ControlCenterVertical);

    pClientChoiceCtrl = new wxChoice(projectDetailsPanel, IDC_CLIENTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pClientChoiceCtrl->SetToolTip(wxT("Select a client to associate this project with"));
    pClientChoiceCtrl->AppendString(wxT("Select a client"));
    pClientChoiceCtrl->SetSelection(0);
    pClientChoiceCtrl->Disable();
    flexGridSizer->Add(pClientChoiceCtrl, common::sizers::ControlDefault);

    flexGridSizer->Add(0, 0);

    pIsDefaultCtrl = new wxCheckBox(projectDetailsPanel, IDC_ISDEFAULT, wxT("Default"));
    pIsDefaultCtrl->SetToolTip(wxT("Enabling this option will pre-select this project when adding tasks"));
    flexGridSizer->Add(pIsDefaultCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        auto isActiveFiller = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT(""));
        flexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(projectDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        flexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);
    }

    /* Billable Controls */
    /* Billable Project Details Box */
    auto billabilityBox = new wxStaticBox(this, wxID_ANY, wxT("Billable Details"));
    auto billabilityBoxSizer = new wxStaticBoxSizer(billabilityBox, wxVERTICAL);
    mainSizer->Add(billabilityBoxSizer, common::sizers::ControlExpand);

    auto billablePanel = new wxPanel(this, wxID_ANY);
    billabilityBoxSizer->Add(billablePanel, common::sizers::ControlExpand);

    auto billableFlexGridSizer = new wxFlexGridSizer(2, 1, 1);
    billablePanel->SetSizer(billableFlexGridSizer);

    /* Billable Checkbox */
    auto billableFiller = new wxStaticText(billablePanel, wxID_STATIC, wxGetEmptyString());
    billableFlexGridSizer->Add(billableFiller, common::sizers::ControlDefault);

    pBillableCtrl = new wxCheckBox(billablePanel, IDC_BILLABLE, wxT("Billable"));
    pBillableCtrl->SetToolTip(wxT("Indicates that work on this project is billable"));
    billableFlexGridSizer->Add(pBillableCtrl, common::sizers::ControlDefault);

    /* Rate Choice Control */
    auto rateText = new wxStaticText(billablePanel, wxID_STATIC, wxT("Rate"));
    billableFlexGridSizer->Add(rateText, common::sizers::ControlDefault);

    pRateChoiceCtrl = new wxChoice(billablePanel, IDC_RATECHOICE, wxDefaultPosition, wxDefaultSize);
    pRateChoiceCtrl->SetToolTip(wxT("Select a rate at which to charge work at"));
    pRateChoiceCtrl->AppendString(wxT("Select a rate"));
    pRateChoiceCtrl->SetSelection(0);
    pRateChoiceCtrl->Disable();
    billableFlexGridSizer->Add(pRateChoiceCtrl, common::sizers::ControlDefault);

    /* Rate Text Control */
    auto rateValueText = new wxStaticText(billablePanel, wxID_STATIC, wxT("Rate Value"));
    billableFlexGridSizer->Add(rateValueText, common::sizers::ControlDefault);

    wxFloatingPointValidator<double> rateValueValidator(2, nullptr, wxNUM_VAL_ZERO_AS_BLANK);
    rateValueValidator.SetRange(0.0, 1000000.0);

    pRateTextCtrl = new wxTextCtrl(billablePanel,
        IDC_RATEVALUE,
        wxGetEmptyString(),
        wxDefaultPosition,
        wxSize(150, -1),
        wxTE_LEFT,
        rateValueValidator);
    pRateTextCtrl->SetHint(wxT("Rate value"));
    pRateTextCtrl->SetToolTip(wxT("Enter the rate at which to charge work at"));
    pRateTextCtrl->Disable();
    billableFlexGridSizer->Add(pRateTextCtrl, common::sizers::ControlDefault);

    /* Currency ComboBox Control */
    auto currencyText = new wxStaticText(billablePanel, wxID_STATIC, wxT("Currency"));
    billableFlexGridSizer->Add(currencyText, common::sizers::ControlDefault);

    wxArrayString choices;
    choices.Add(wxT("Select a currency"));
    pCurrencyComboBoxCtrl = new wxComboBox(
        billablePanel, IDC_CURRENCYCHOICE, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_DROPDOWN);
    pCurrencyComboBoxCtrl->SetToolTip(wxT("Select a currency to associate the rate value with"));
    pCurrencyComboBoxCtrl->SetSelection(0);
    pCurrencyComboBoxCtrl->Disable();
    billableFlexGridSizer->Add(pCurrencyComboBoxCtrl, common::sizers::ControlExpand);

    /* Hours Text Control */
    auto hoursLabel = new wxStaticText(billablePanel, wxID_STATIC, wxT("Hours"));
    billableFlexGridSizer->Add(hoursLabel, common::sizers::ControlDefault);

    wxTextValidator hoursValidator(wxFILTER_DIGITS);
    pHoursTextCtrl = new wxTextCtrl(
        billablePanel, IDC_HOURS, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, hoursValidator);
    pHoursTextCtrl->SetHint(wxT("Enter hours"));
    pHoursTextCtrl->SetToolTip(wxT("Enter number of hours to charge against a daily rate for a project"));
    pHoursTextCtrl->Disable();
    billableFlexGridSizer->Add(pHoursTextCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        /* Date Text Control */
        pDateTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Created on: %s"));
        auto font = pDateTextCtrl->GetFont();
        font.SetPointSize(8);
        pDateTextCtrl->SetFont(font);
        mainSizer->Add(pDateTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    }

    /* Horizontal Line*/
    auto separation_line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("OK"));
    pOkButton->SetDefault();
    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("Cancel"));

    buttonPanelSizer->Add(pOkButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(pCancelButton, common::sizers::ControlDefault);
}

// clang-format off
void ProjectDialog::ConfigureEventBindings()
{
    pNameTextCtrl->Bind(
        wxEVT_TEXT,
        &ProjectDialog::OnNameChange,
        this
    );

    pEmployerChoiceCtrl->Bind(
        wxEVT_CHOICE,
        &ProjectDialog::OnEmployerChoiceSelection,
        this
    );

    pBillableCtrl->Bind(
        wxEVT_CHECKBOX,
        &ProjectDialog::OnBillableCheck,
        this
    );

    pRateChoiceCtrl->Bind(
        wxEVT_CHOICE,
        &ProjectDialog::OnRateChoiceSelection,
        this
    );

    if (bIsEdit) {
        pIsActiveCtrl->Bind(
            wxEVT_CHECKBOX,
            &ProjectDialog::OnIsActiveCheck,
            this
        );
    }

    pOkButton->Bind(
        wxEVT_BUTTON,
        &ProjectDialog::OnOk,
        this,
        wxID_OK
    );

    pCancelButton->Bind(
        wxEVT_BUTTON,
        &ProjectDialog::OnCancel,
        this,
        wxID_CANCEL
    );
}
// clang-format on

void ProjectDialog::FillControls()
{
    /* Load Employers */
    std::vector<std::unique_ptr<model::EmployerModel>> employers;
    try {
        employers = model::EmployerModel::GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in EmployerModel::GetAll()() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& employer : employers) {
        pEmployerChoiceCtrl->Append(employer->GetName(), util::IntToVoidPointer(employer->GetEmployerId()));
    }

    /* Load Rate Types */
    std::vector<std::unique_ptr<model::RateTypeModel>> rateTypes;
    try {
        rateTypes = model::RateTypeModel::GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in RateTypeModel::GetAll() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& rateType : rateTypes) {
        pRateChoiceCtrl->Append(rateType->GetName(), util::IntToVoidPointer(rateType->GetRateTypeId()));
    }

    /* Load Currencies */
    std::vector<std::unique_ptr<model::CurrencyModel>> curriencies;
    try {
        curriencies = model::CurrencyModel::GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in CurrencyModel::GetAll() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& currency : curriencies) {
        pCurrencyComboBoxCtrl->Append(wxString::Format(wxT("%s (%s)"), currency->GetName(), currency->GetCode()),
            util::IntToVoidPointer(currency->GetCurrencyId()));
    }
}

void ProjectDialog::DataToControls()
{
    std::unique_ptr<model::ProjectModel> project;

    try {
        project = model::ProjectModel::GetById(mProjectId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectModel::GetById() - {0:d} : {1}", e.get_code(), e.what());
    }

    /* Set project names */
    pNameTextCtrl->ChangeValue(project->GetName());
    pDisplayNameCtrl->ChangeValue(project->GetDisplayName());

    /* Set employer associated with project */
    pEmployerChoiceCtrl->SetStringSelection(project->GetEmployer()->GetName());

    /* Set client (if any) associated with project */
    if (project->HasClientLinked()) {
        pClientChoiceCtrl->Clear();
        pClientChoiceCtrl->AppendString(wxT("Select a client"));
        std::vector<std::unique_ptr<model::ClientModel>> clients;
        try {
            clients = model::ClientModel::GetByEmployerId(project->GetEmployer()->GetEmployerId());
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in ClientModel::GetByEmployerId() - {0:d} : {1}", e.get_code(), e.what());
        }

        for (const auto& client : clients) {
            pClientChoiceCtrl->Append(client->GetName(), util::IntToVoidPointer(client->GetClientId()));
        }

        pClientChoiceCtrl->SetSelection(0);
        if (!pClientChoiceCtrl->IsEnabled()) {
            pClientChoiceCtrl->Enable();
        }
        pClientChoiceCtrl->SetStringSelection(project->GetClient()->GetName());
    }

    /* Set project billable check */
    pBillableCtrl->SetValue(project->IsBillable());

    /* Set project is default check */
    pIsDefaultCtrl->SetValue(project->IsDefault());

    if (project->IsBillableWithUnknownRateScenario()) {
        /* Project is billable, set the billable attributes */
        pRateChoiceCtrl->Enable();
        /* Set the rate choice */
        pRateChoiceCtrl->SetStringSelection(project->GetRateType()->GetName());
    }

    if (project->IsBillableScenarioWithHourlyRate()) {
        /* Project is billable, set the billable attributes */
        pRateChoiceCtrl->Enable();
        /* Set the rate choice */
        pRateChoiceCtrl->SetStringSelection(project->GetRateType()->GetName());
        pRateTextCtrl->Enable();
        /* Set the rate value */
        pRateTextCtrl->SetValue(wxString::Format(wxT("%.2f"), wxString(std::to_string(*project->GetRate()))));

        pCurrencyComboBoxCtrl->Enable();
        /* Set the currency for project */
        pCurrencyComboBoxCtrl->SetStringSelection(
            wxString::Format(wxT("%s (%s)"), project->GetCurrency()->GetName(), project->GetCurrency()->GetCode()));

        pHoursTextCtrl->Disable();
    }

    if (project->IsBillableScenarioWithDailyRate()) {
        /* Project is billable, set the billable attributes */

        /* Set the rate choice */
        pRateChoiceCtrl->Enable();
        pRateChoiceCtrl->SetStringSelection(project->GetRateType()->GetName());

        /* Set the rate value */
        pRateTextCtrl->Enable();
        pRateTextCtrl->SetValue(wxString::Format(wxT("%.2f"), wxString(std::to_string(*project->GetRate()))));

        /* Set the currency */
        pCurrencyComboBoxCtrl->Enable();
        pCurrencyComboBoxCtrl->SetStringSelection(
            wxString::Format(wxT("%s (%s)"), project->GetCurrency()->GetName(), project->GetCurrency()->GetCode()));

        /* Set the hours */
        pHoursTextCtrl->ChangeValue(wxString(std::to_string(*project->GetHours())));
        pHoursTextCtrl->Enable();
    }

    /* Is the project active (i.e. not deleted) */
    pIsActiveCtrl->SetValue(project->IsActive());

    /* When was the project created and modified */
    pDateTextCtrl->SetLabel(wxString::Format(ProjectDialog::DateLabel,
        project->GetDateCreated().FormatISOCombined(),
        project->GetDateModified().FormatISOCombined()));
}

void ProjectDialog::OnNameChange(wxCommandEvent& event)
{
    wxString name = pNameTextCtrl->GetValue();
    pDisplayNameCtrl->ChangeValue(name);
}

void ProjectDialog::OnEmployerChoiceSelection(wxCommandEvent& event)
{
    int employerId = util::VoidPointerToInt(pEmployerChoiceCtrl->GetClientData(pEmployerChoiceCtrl->GetSelection()));

    pClientChoiceCtrl->Clear();
    pClientChoiceCtrl->AppendString(wxT("Select a client"));
    pClientChoiceCtrl->SetSelection(0);

    if (event.GetSelection() == 0) {
        pClientChoiceCtrl->Disable();
    } else {
        std::vector<std::unique_ptr<model::ClientModel>> clients;
        try {
            clients = model::ClientModel::GetByEmployerId(employerId);
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in ClientModel::GetByEmployerId() - {0:d} : {1}", e.get_code(), e.what());
        }

        if (!clients.empty()) {
            for (const auto& client : clients) {
                pClientChoiceCtrl->Append(client->GetName(), util::IntToVoidPointer(client->GetClientId()));
            }

            if (!pClientChoiceCtrl->IsEnabled()) {
                pClientChoiceCtrl->Enable();
            }
        } else {
            pClientChoiceCtrl->Disable();
        }
    }
}

void ProjectDialog::OnBillableCheck(wxCommandEvent& event)
{
    pProject->IsBillable(event.IsChecked());
    if (event.IsChecked()) {
        pRateChoiceCtrl->Enable();
    } else {
        pRateChoiceCtrl->Disable();
        pRateTextCtrl->Disable();
        pCurrencyComboBoxCtrl->Disable();
        pHoursTextCtrl->Disable();

        {
            pProject->SwitchOutOfBillableScenario();

            pRateChoiceCtrl->SetSelection(0);
            pRateTextCtrl->ChangeValue(wxGetEmptyString());
            pCurrencyComboBoxCtrl->SetSelection(0);
            pHoursTextCtrl->ChangeValue(wxGetEmptyString());
        }
    }
}

void ProjectDialog::OnRateChoiceSelection(wxCommandEvent& event)
{
    int selection = util::VoidPointerToInt(pRateChoiceCtrl->GetClientData(pRateChoiceCtrl->GetSelection()));
    if (selection == static_cast<int>(constants::RateTypes::Unknown) || selection == 0) {
        pRateTextCtrl->Disable();
        pCurrencyComboBoxCtrl->Disable();

        pRateTextCtrl->ChangeValue(wxGetEmptyString());
        pCurrencyComboBoxCtrl->SetSelection(0);
        pHoursTextCtrl->Disable();
    }
    if (selection == static_cast<int>(constants::RateTypes::Hourly)) {
        pRateTextCtrl->Enable();
        pCurrencyComboBoxCtrl->Enable();
        pHoursTextCtrl->Disable();
    }
    if (selection == static_cast<int>(constants::RateTypes::Daily)) {
        pRateTextCtrl->Enable();
        pCurrencyComboBoxCtrl->Enable();
        pHoursTextCtrl->Enable();
    }
}

void ProjectDialog::OnIsDefaultCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        model::ProjectModel::UnmarkDefaultProjects();
    }
}

void ProjectDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    if (TryTransferValuesFromControls()) {
        if (!bIsEdit) {
            try {
                model::ProjectModel::Create(std::move(pProject));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in ProjectModel::Create() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            try {
                model::ProjectModel::Update(std::move(pProject));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in ProjectModel::Update() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            try {
                model::ProjectModel::Delete(std::move(pProject));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in ProjectModel::Delete() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        EndModal(wxID_OK);
    }
}

void ProjectDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

void ProjectDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pNameTextCtrl->Enable();
        pDisplayNameCtrl->Enable();
        pEmployerChoiceCtrl->Enable();

        if (pProject->HasClientLinked()) {
            pClientChoiceCtrl->Enable();
        }

        pBillableCtrl->Enable();
        pIsDefaultCtrl->Enable();

        if (pProject->IsBillableWithUnknownRateScenario()) {
            pRateChoiceCtrl->Enable();
        }

        if (pProject->IsBillableScenarioWithHourlyRate()) {
            pRateChoiceCtrl->Enable();
            pRateTextCtrl->Enable();
            pCurrencyComboBoxCtrl->Enable();
        }

        if (pProject->IsBillableScenarioWithDailyRate()) {
            pRateChoiceCtrl->Enable();
            pRateTextCtrl->Enable();
            pCurrencyComboBoxCtrl->Enable();
            pHoursTextCtrl->Enable();
        }

    } else {
        pNameTextCtrl->Disable();
        pDisplayNameCtrl->Disable();
        pEmployerChoiceCtrl->Disable();
        pClientChoiceCtrl->Disable();
        pBillableCtrl->Disable();
        pIsDefaultCtrl->Disable();
        pRateChoiceCtrl->Disable();
        pRateTextCtrl->Disable();
        pCurrencyComboBoxCtrl->Disable();
        pHoursTextCtrl->Disable();
    }
}

bool ProjectDialog::TryTransferValuesFromControls()
{
    wxString name = pNameTextCtrl->GetValue();
    if (name.empty() || name.length() < 2 || name.length() > 255) {
        common::validations::ForRequiredText(pNameTextCtrl, wxT("project name"));
        return false;
    }

    pProject->SetName(name);

    wxString displayName = pDisplayNameCtrl->GetValue();
    if (displayName.empty() || displayName.length() < 2 || displayName.length() > 255) {
        common::validations::ForRequiredText(pDisplayNameCtrl, wxT("display name"));
        return false;
    }
    pProject->SetDisplayName(displayName);

    if (pEmployerChoiceCtrl->GetSelection() == 0) {
        common::validations::ForRequiredChoiceSelection(pEmployerChoiceCtrl, wxT("employer"));
        return false;
    }
    pProject->SetEmployerId(
        util::VoidPointerToInt(pEmployerChoiceCtrl->GetClientData(pEmployerChoiceCtrl->GetSelection())));

    if (pClientChoiceCtrl->IsEnabled()) {
        pProject->SetClientId(
            util::VoidPointerToInt(pClientChoiceCtrl->GetClientData(pClientChoiceCtrl->GetSelection())));
    }

    pProject->IsBillable(pBillableCtrl->GetValue());
    pProject->IsDefault(pIsDefaultCtrl->GetValue());

    if (pProject->IsBillable()) {
        if (pRateChoiceCtrl->GetSelection() == 0) {
            common::validations::ForRequiredChoiceSelection(pRateChoiceCtrl, wxT("rate"));
            return false;
        }
        pProject->SetRateTypeId(
            util::VoidPointerToInt(pRateChoiceCtrl->GetClientData(pRateChoiceCtrl->GetSelection())));

        int selection = pRateChoiceCtrl->GetSelection();
        if (selection == static_cast<int>(constants::RateTypes::Unknown)) {
            // nothing to do
        }
        if (selection == static_cast<int>(constants::RateTypes::Hourly)) {
            wxString value = pRateTextCtrl->GetValue();
            if (value.empty()) {
                common::validations::ForRequiredNumber(pRateTextCtrl, wxT("Rate amount"));
                return false;
            }
            pProject->SetRate(std::move(std::make_unique<double>(std::stod(pRateTextCtrl->GetValue().ToStdString()))));
            if (pCurrencyComboBoxCtrl->GetSelection() == 0) {
                common::validations::ForRequiredChoiceSelection(pCurrencyComboBoxCtrl, wxT("currency"));
                return false;
            }
            pProject->SetCurrencyId(
                util::VoidPointerToInt(pCurrencyComboBoxCtrl->GetClientData(pCurrencyComboBoxCtrl->GetSelection())));
        }
        if (selection == static_cast<int>(constants::RateTypes::Daily)) {
            wxString value = pRateTextCtrl->GetValue();
            if (value.empty()) {
                common::validations::ForRequiredNumber(pRateTextCtrl, wxT("Rate amount"));
                return false;
            }
            pProject->SetRate(std::move(std::make_unique<double>(std::stod(value.ToStdString()))));
            if (pCurrencyComboBoxCtrl->GetSelection() == 0) {
                common::validations::ForRequiredChoiceSelection(pCurrencyComboBoxCtrl, wxT("currency"));
                return false;
            }
            pProject->SetCurrencyId(
                util::VoidPointerToInt(pCurrencyComboBoxCtrl->GetClientData(pCurrencyComboBoxCtrl->GetSelection())));
            wxString hourValue = pHoursTextCtrl->GetValue();
            if (hourValue.empty()) {
                common::validations::ForRequiredNumber(pRateTextCtrl, wxT("Hours value"));
                return false;
            }
            pProject->SetHours(std::move(std::make_unique<int>(std::stoi(hourValue.ToStdString()))));
        }
    }

    return true;
}

} // namespace app::dlg
