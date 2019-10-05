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

#include "clientdlg.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/util.h"
#include "../services/db_service.h"

namespace app::dialog
{
// clang-format off
wxBEGIN_EVENT_TABLE(ClientDialog, wxDialog)
EVT_BUTTON(wxID_OK, ClientDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, ClientDialog::OnCancel)
EVT_CHECKBOX(ClientDialog::IDC_ISACTIVE, ClientDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

ClientDialog::ClientDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    const wxString& name)
    : pLogger(logger)
    , mNameText(wxGetEmptyString())
    , mEmployerId(-1)
    , bIsEdit(false)
    , mClientId(0)
// clang-format on
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
    , mNameText(wxGetEmptyString())
    , mEmployerId(-1)
    , bIsEdit(isEdit)
    , mClientId(clientId)
{
    Create(parent,
        wxID_ANY,
        wxT("Edit Client"),
        wxDefaultPosition,
        wxSize(320, 320),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU,
        name);
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
        FillControls();

        if (bIsEdit) {
            DataToControls();
        }

        GetSizer()->Fit(this);
        SetIcon(common::GetProgramIcon());
        Centre();
    }

    return created;
}

void ClientDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Client Information"));
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

    pNameCtrl =
        new wxTextCtrl(clientDetailsPanel, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT);
    pNameCtrl->Bind(wxEVT_KILL_FOCUS, &ClientDialog::OnClientTextControlLostFocus, this);
    pNameCtrl->SetToolTip(wxT("Enter a name for the client"));
    taskFlexGridSizer->Add(pNameCtrl, common::sizers::ControlDefault);

    /* Employer Selection Control */
    auto employerNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Employer"));
    taskFlexGridSizer->Add(employerNameText, common::sizers::ControlCenterVertical);

    pEmployerChoiceCtrl =
        new wxChoice(clientDetailsPanel, IDC_EMPLOYER, wxDefaultPosition, wxDefaultSize);
    pEmployerChoiceCtrl->AppendString(wxT("Select a employer"));
    pEmployerChoiceCtrl->SetSelection(0);
    pEmployerChoiceCtrl->SetToolTip(wxT("Select a employer to associate the client with"));
    taskFlexGridSizer->Add(pEmployerChoiceCtrl, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    if (bIsEdit) {
        auto isActiveFiller = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT(""));
        taskFlexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(clientDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
        taskFlexGridSizer->Add(pIsActiveCtrl, common::sizers::ControlDefault);

        /* Date Created Text Control */
        pDateCreatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Created on: %s"));
        auto font = pDateCreatedTextCtrl->GetFont();
        font.MakeItalic();
        font.SetPointSize(8);
        pDateCreatedTextCtrl->SetFont(font);
        detailsBoxSizer->Add(pDateCreatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        /* Date Updated Text Control */
        pDateUpdatedTextCtrl = new wxStaticText(this, wxID_STATIC, wxT("Updated on: %s"));
        detailsBoxSizer->Add(pDateUpdatedTextCtrl, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 10);
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
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(pOkButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(cancelButton, wxSizerFlags().Border(wxALL, 5));
}

void ClientDialog::FillControls()
{
    services::db_service dbService;
    std::vector<models::employer> employers;
    try {
        employers = dbService.get_employers();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on get_employers() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (auto employer : employers) {
        pEmployerChoiceCtrl->Append(employer.employer_name, util::IntToVoidPointer(employer.employer_id));
    }
}

void ClientDialog::DataToControls()
{
    services::db_service dbService;
    models::client client;
    try {
        client = dbService.get_client_by_id(mClientId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on get_client_by_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    pNameCtrl->ChangeValue(client.client_name);
    pEmployerChoiceCtrl->SetStringSelection(client.employer_name);

    wxString dateCreatedString = util::ConvertUnixTimestampToString(client.date_created_utc);
    wxString dateCreatedLabel = pDateCreatedTextCtrl->GetLabelText();
    pDateCreatedTextCtrl->SetLabel(wxString::Format(dateCreatedLabel, dateCreatedString));

    wxString dateUpdatedString = util::ConvertUnixTimestampToString(client.date_modified_utc);
    wxString dateUpdatedLabel = pDateUpdatedTextCtrl->GetLabelText();
    pDateUpdatedTextCtrl->SetLabel(wxString::Format(dateUpdatedLabel, dateUpdatedString));

    pIsActiveCtrl->SetValue(client.is_active);
}

bool ClientDialog::Validate()
{
    if (mNameText.empty()) {
        auto message = wxString::Format(Constants::Messages::IsEmpty, wxT("Client name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mEmployerId == -1 || mEmployerId == 0) {
        auto message = wxString::Format(Constants::Messages::SelectionRequired, wxT("Employer"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }
    return true;
}

bool ClientDialog::AreControlsEmpty()
{
    bool isEmpty = mNameText.empty() && (mEmployerId == -1 || mEmployerId == 0);
    return isEmpty;
}

void ClientDialog::OnClientTextControlLostFocus(wxFocusEvent& event)
{
    if (pNameCtrl->GetValue().length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Client name"), 2);
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        pOkButton->Disable();

    } else if (pNameCtrl->GetValue().length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Client name"), 255);
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        pOkButton->Disable();
    } else {
        pOkButton->Enable();
    }

    event.Skip();
}

void ClientDialog::OnOk(wxCommandEvent& event)
{
    mNameText = pNameCtrl->GetValue();
    mEmployerId = util::VoidPointerToInt(pEmployerChoiceCtrl->GetClientData(pEmployerChoiceCtrl->GetSelection()));

    bool isValid = Validate();
    if (!isValid) {
        return;
    }

    services::db_service clientService;
    try {
        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            models::client client;
            client.client_id = mClientId;
            client.client_name = std::string(mNameText.ToUTF8());
            client.date_modified_utc = util::UnixTimestamp();
            client.employer_id = mEmployerId;
            clientService.update_client(client);
        }
        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            clientService.delete_client(mClientId, util::UnixTimestamp());
        }
        if (!bIsEdit) {
            clientService.create_new_client(std::string(mNameText.ToUTF8()), mEmployerId);
        }
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured on client OnSave() - {0:d} : {1}", e.get_code(), e.what());
    }

    EndModal(wxID_OK);
}

void ClientDialog::OnCancel(wxCommandEvent& event)
{
    bool areControlsEmpty = AreControlsEmpty();
    if (!areControlsEmpty) {
        int answer = wxMessageBox(wxT("Are you sure you want to exit?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}

void ClientDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pNameCtrl->Enable();
        pEmployerChoiceCtrl->Enable();
    } else {
        pNameCtrl->Disable();
        pEmployerChoiceCtrl->Disable();
    }
}
} // namespace app::dialog
