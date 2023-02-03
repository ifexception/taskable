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
#include "../common/resources.h"
#include "../common/util.h"

#include "../data/employerdata.h"

#include "../models/employermodel.h"
#include "../models/clientmodel.h"

namespace app::dlg
{
ProjectDialog::ProjectDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , pNameTextCtrl(nullptr)
    , pDisplayNameCtrl(nullptr)
    , pEmployerChoiceCtrl(nullptr)
    , pClientChoiceCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pIsDefaultCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , pProject(std::make_unique<model::ProjectModel>())
    , mProjectId(0)
    , bIsEdit(false)
    , mClientData()
    , mProjectData()
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
    , pIsActiveCtrl(nullptr)
    , pIsDefaultCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , pProject(std::make_unique<model::ProjectModel>(projectId))
    , mProjectId(projectId)
    , bIsEdit(isEdit)
    , mClientData()
    , mProjectData()
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
        SetIcon(rc::GetProgramIcon());
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
    data::EmployerData employerData;
    std::vector<std::unique_ptr<model::EmployerModel>> employers;
    try {
        employers = employerData.GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in EmployerData::GetAll()() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& employer : employers) {
        pEmployerChoiceCtrl->Append(employer->GetName(), util::IntToVoidPointer(employer->GetEmployerId()));
    }
}

void ProjectDialog::DataToControls()
{
    std::unique_ptr<model::ProjectModel> project;

    try {
        project = mProjectData.GetById(mProjectId);
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
            clients = mClientData.GetByEmployerId(project->GetEmployer()->GetEmployerId());
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

    /* Is the project active (i.e. not deleted) */
    pIsActiveCtrl->SetValue(project->IsActive());

    /* When was the project created and modified */
    pDateTextCtrl->SetLabel(wxString::Format(constants::DateLabel,
        util::ToFriendlyDateTimeString(project->GetDateCreated()),
        util::ToFriendlyDateTimeString(project->GetDateModified())));
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
            clients = mClientData.GetByEmployerId(employerId);
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

void ProjectDialog::OnIsDefaultCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        mProjectData.UnmarkDefaultProjects();
    }
}

void ProjectDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    if (TryTransferValuesFromControls()) {
        if (!bIsEdit) {
            try {
                mProjectData.Create(std::move(pProject));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in ProjectModel::Create() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            try {
                mProjectData.Update(std::move(pProject));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in ProjectModel::Update() - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            try {
                mProjectData.Delete(mProjectId);
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

        pIsDefaultCtrl->Enable();

    } else {
        pNameTextCtrl->Disable();
        pDisplayNameCtrl->Disable();
        pEmployerChoiceCtrl->Disable();
        pClientChoiceCtrl->Disable();
        pIsDefaultCtrl->Disable();
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
    pProject->IsDefault(pIsDefaultCtrl->GetValue());

    return true;
}

} // namespace app::dlg
