//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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

#include "clientdialog.h"

#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../db/database_exception.h"
#include "../services/db_service.h"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(ClientDialog, wxDialog);

wxBEGIN_EVENT_TABLE(ClientDialog, wxDialog)
    EVT_BUTTON(ids::ID_SAVE, ClientDialog::OnSave)
    EVT_BUTTON(wxID_CANCEL, ClientDialog::OnCancel)
    EVT_CHECKBOX(ClientDialog::IDC_ISACTIVE, ClientDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

ClientDialog::ClientDialog(wxWindow* parent, bool isEdit, int clientId, const wxString& name)
    : mNameText(wxGetEmptyString())
    , mEmployerId(-1)
    , bIsEdit(isEdit)
    , mClientId(clientId)
{
    long style = wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU;
    wxString title;
    wxSize size;
    if (isEdit) {
        title = wxT("Edit Client");
        size.Set(320, 320);
    } else {
        title = wxT("Add Client");
        size.Set(320, 240);
    }

    Create(parent, wxID_ANY, title, wxDefaultPosition, size, style, name);
}

bool ClientDialog::Create(wxWindow* parent, wxWindowID windowId, const wxString& title, const wxPoint& position, const wxSize& size, long style, const wxString& name)
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
        Center();
    }

    return created;
}

void ClientDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Task Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Client Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpandProp);

    auto clientDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(clientDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    clientDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Client Name Control */
    auto clientNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(clientNameText, common::sizers::ControlCenterVertical);

    pNameCtrl = new wxTextCtrl(clientDetailsPanel, IDC_NAME, wxGetEmptyString(), wxDefaultPosition, wxSize(150, -1), wxTE_LEFT, wxDefaultValidator, wxT("client_name_ctrl"));
    pNameCtrl->SetToolTip(wxT("Enter a name for the client"));
    taskFlexGridSizer->Add(pNameCtrl, common::sizers::ControlDefault);

    /* Employer Selection Control */
    auto employerNameText = new wxStaticText(clientDetailsPanel, wxID_STATIC, wxT("Employer"));
    taskFlexGridSizer->Add(employerNameText, common::sizers::ControlCenterVertical);

    pEmployerChoiceCtrl = new wxComboBox(clientDetailsPanel, IDC_EMPLOYER, wxT("Select a employer"), wxDefaultPosition, wxDefaultSize);
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
    auto separationLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1), wxLI_HORIZONTAL, wxT("new_task_static_line"));
    mainSizer->Add(separationLine, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, wxSizerFlags(wxSizerFlags().Border(wxALL, 5)).Center());

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, wxSizerFlags().Border(wxALL, 5));
    buttonPanelSizer->Add(cancelButton, wxSizerFlags().Border(wxALL, 5));
}

void ClientDialog::FillControls()
{
    services::db_service dbService;
    std::vector<models::employer> employers;
    try {
        employers = dbService.get_employers();
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    for (auto employer : employers) {
        pEmployerChoiceCtrl->Append(employer.employer_name, (void*) employer.employer_id);
    }
}

void ClientDialog::DataToControls()
{
    services::db_service dbService;
    models::client client;
    try {
        client = dbService.get_client_by_id(mClientId);
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }

    pNameCtrl->SetValue(client.client_name);
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

    if (mNameText.length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Client name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mNameText.length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Client name"));
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

void ClientDialog::OnSave(wxCommandEvent & event)
{
    mNameText = pNameCtrl->GetValue();
    mEmployerId = (int) pEmployerChoiceCtrl->GetClientData(pEmployerChoiceCtrl->GetSelection()); // FIXME: loss of precision -> convert to intptr_t and then to int

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
    } catch (const db::database_exception& e) {
        // TODO Log exception
    }
    EndModal(ids::ID_SAVE);
}

void ClientDialog::OnCancel(wxCommandEvent& event)
{
    bool areControlsEmpty = AreControlsEmpty();
    if (!areControlsEmpty) {
        int answer = wxMessageBox(wxT("Are you sure you want to exit?"), wxT("Confirm"),
            wxYES_NO | wxICON_QUESTION);
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

}
