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

#include "clientdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/richtooltip.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/resources.h"
#include "../common/util.h"
#include "../models/employermodel.h"

namespace app::dlg
{
ClientDialog::ClientDialog(wxWindow* parent, std::shared_ptr<spdlog::logger> logger, const wxString& name)
    : pLogger(logger)
    , pNameTextCtrl(nullptr)
    , pEmployerChoiceCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , pClient(std::make_unique<model::ClientModel>())
    , mClientId(-1)
    , bIsEdit(false)
    , mEmployerData()
    , mClientData()
{
    Create(parent,
        wxID_ANY,
        wxT("Add Client"),
        wxDefaultPosition,
        wxSize(320, 240),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
    SetMinClientSize(wxSize(320, 240));
}

ClientDialog::ClientDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    bool isEdit,
    int clientId,
    const wxString& name)
    : pLogger(logger)
    , pNameTextCtrl(nullptr)
    , pEmployerChoiceCtrl(nullptr)
    , pIsActiveCtrl(nullptr)
    , pDateTextCtrl(nullptr)
    , pOkButton(nullptr)
    , pCancelButton(nullptr)
    , pClient(std::make_unique<model::ClientModel>(clientId))
    , mClientId(clientId)
    , bIsEdit(isEdit)
    , mEmployerData()
    , mClientData()
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Client"),
        wxDefaultPosition,
        wxSize(320, 320),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
    SetMinClientSize(wxSize(320, 240));
}

bool ClientDialog::Create(wxWindow* parent,
    wxWindowID windowId,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style,
    const wxString& name)
{
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

void ClientDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Client Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    mainSizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto clientDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(clientDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    clientDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Client Name Control */
    auto clientNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(clientNameText, common::sizers::ControlCenterVertical);

    wxTextValidator nameValidator(wxFILTER_ALPHANUMERIC | wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString allowedChars;
    allowedChars.Add(wxT(" "));
    nameValidator.SetIncludes(allowedChars);

    pNameTextCtrl = new wxTextCtrl(
        clientDetailsPanel, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, nameValidator);
    pNameTextCtrl->SetHint(wxT("Client name"));
    pNameTextCtrl->SetToolTip(wxT("Enter a name for the client"));
    taskFlexGridSizer->Add(pNameTextCtrl, common::sizers::ControlDefault);

    /* Employer Selection Control */
    auto employerNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Employer"));
    taskFlexGridSizer->Add(employerNameText, common::sizers::ControlCenterVertical);

    pEmployerChoiceCtrl = new wxChoice(clientDetailsPanel, IDC_EMPLOYERCHOICE, wxDefaultPosition, wxDefaultSize);
    pEmployerChoiceCtrl->AppendString(wxT("Select a employer"));
    pEmployerChoiceCtrl->SetSelection(0);
    pEmployerChoiceCtrl->SetToolTip(wxT("Select a employer to associate the client with"));
    taskFlexGridSizer->Add(pEmployerChoiceCtrl, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    if (bIsEdit) {
        /* Is Active Checkbox Control */
        auto isActiveFiller = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT(""));
        taskFlexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        pIsActiveCtrl = new wxCheckBox(clientDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        taskFlexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);

        /* Date Text Control */
        pDateTextCtrl = new wxStaticText(this, wxID_STATIC, wxT(""));
        auto font = pDateTextCtrl->GetFont();
        font.SetPointSize(8);
        pDateTextCtrl->SetFont(font);
        detailsBoxSizer->Add(pDateTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    }

    /* Horizontal Line*/
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL);
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    pOkButton = new wxButton(buttonPanel, wxID_OK, wxT("&OK"));
    pOkButton->SetDefault();
    pCancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pOkButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(pCancelButton, wxSizerFlags().Border(wxALL, 5));
}

// clang-format off
void ClientDialog::ConfigureEventBindings()
{
    if (bIsEdit) {
        pIsActiveCtrl->Bind(
            wxEVT_CHECKBOX,
            &ClientDialog::OnIsActiveCheck,
            this
        );
    }

    pOkButton->Bind(
        wxEVT_BUTTON,
        &ClientDialog::OnOk,
        this,
        wxID_OK
    );

    pCancelButton->Bind(
        wxEVT_BUTTON,
        &ClientDialog::OnCancel,
        this,
        wxID_CANCEL
    );
}
// clang-format on

void ClientDialog::FillControls()
{
    std::vector<std::unique_ptr<model::EmployerModel>> employers;
    try {
        employers = mEmployerData.GetAll();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on EmployerData::GetAll() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (const auto& employer : employers) {
        pEmployerChoiceCtrl->Append(employer->GetName(), util::IntToVoidPointer(employer->GetEmployerId()));
    }
}

void ClientDialog::DataToControls()
{
    std::unique_ptr<model::ClientModel> client = nullptr;
    try {
        client = mClientData.GetById(mClientId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on ClientData::GetById() - {0:d} : {1}", e.get_code(), e.what());
    }

    pNameTextCtrl->ChangeValue(client->GetName());

    pEmployerChoiceCtrl->SetStringSelection(client->GetEmployer()->GetName());

    pDateTextCtrl->SetLabel(wxString::Format(constants::DateLabel,
        util::ToFriendlyDateTimeString(client->GetDateCreated()),
        util::ToFriendlyDateTimeString(client->GetDateModified())));

    pIsActiveCtrl->SetValue(client->IsActive());
}

bool ClientDialog::TransferDataAndValidate()
{
    wxString name = pNameTextCtrl->GetValue();
    if (name.empty() || name.length() < 2 || name.length() > 255) {
        common::validations::ForRequiredText(pNameTextCtrl, wxT("client name"));
        return false;
    }

    pClient->SetName(name);

    int employerId = util::VoidPointerToInt(pEmployerChoiceCtrl->GetClientData(pEmployerChoiceCtrl->GetSelection()));
    if (employerId < 1) {
        common::validations::ForRequiredChoiceSelection(pEmployerChoiceCtrl, wxT("employer"));
        return false;
    }
    pClient->SetEmployerId(employerId);

    return true;
}

void ClientDialog::OnOk(wxCommandEvent& event)
{
    if (TransferDataAndValidate()) {
        if (!bIsEdit) {
            try {
                mClientData.Create(std::move(pClient));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in category ClientData::Create - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            try {
                mClientData.Update(std::move(pClient));
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in category ClientData::Update - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            try {
                mClientData.Delete(mClientId);
            } catch (const sqlite::sqlite_exception& e) {
                pLogger->error("Error occured in category ClientData::Delete - {0:d} : {1}", e.get_code(), e.what());
                EndModal(ids::ID_ERROR_OCCURED);
            }
        }

        EndModal(wxID_OK);
    }
}

void ClientDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void ClientDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pNameTextCtrl->Enable();
        pEmployerChoiceCtrl->Enable();
    } else {
        pNameTextCtrl->Disable();
        pEmployerChoiceCtrl->Disable();
    }
}
} // namespace app::dlg
