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

#include "projectdialog.h"

#include <sqlite_modern_cpp/errors.h>
#include <wx/statline.h>

#include "../common/constants.h"
#include "../common/common.h"
#include "../common/ids.h"
#include "../common/util.h"
#include "../services/db_service.h"

namespace app::dialog
{
// clang-format off
wxBEGIN_EVENT_TABLE(ProjectDialog, wxDialog)
EVT_BUTTON(ids::ID_SAVE, ProjectDialog::OnSave)
EVT_BUTTON(wxID_CANCEL, ProjectDialog::OnCancel)
EVT_CHOICE(ProjectDialog::IDC_EMPLOYERCHOICE, ProjectDialog::OnEmployerSelect)
EVT_CHECKBOX(ProjectDialog::IDC_ISACTIVE, ProjectDialog::OnIsActiveCheck)
wxEND_EVENT_TABLE()

ProjectDialog::ProjectDialog(wxWindow* parent,
    std::shared_ptr<spdlog::logger> logger,
    bool isEdit,
    int projectId,
    const wxString& name)
    : pLogger(logger)
    , mNameText(wxGetEmptyString())
    , mDisplayNameText(wxGetEmptyString())
    , mEmployerId(-1)
    , mClientId(-1)
    , mProjectId(projectId)
    , bIsEdit(isEdit)
// clang-format on
{
    wxString title;
    wxSize size;
    if (isEdit) {
        title = wxT("Edit Project");
        size.Set(420, 380);
    } else {
        title = wxT("Add Project");
        size.Set(420, 440);
    }

    Create(parent, wxID_ANY, title, wxDefaultPosition, size, wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU, name);
}

bool ProjectDialog::Create(wxWindow* parent,
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
        Center();
    }

    return created;
}

void ProjectDialog::CreateControls()
{
    /* Window Sizing */
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(mainPanelSizer, common::sizers::ControlDefault);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    mainPanelSizer->Add(sizer, 0);

    /* Project Details Box */
    auto detailsBox = new wxStaticBox(this, wxID_ANY, wxT("Project Details"));
    auto detailsBoxSizer = new wxStaticBoxSizer(detailsBox, wxVERTICAL);
    sizer->Add(detailsBoxSizer, common::sizers::ControlExpand);

    auto projectDetailsPanel = new wxPanel(this, wxID_STATIC);
    detailsBoxSizer->Add(projectDetailsPanel, common::sizers::ControlExpand);

    auto taskFlexGridSizer = new wxFlexGridSizer(0, 2, 0, 0);
    projectDetailsPanel->SetSizer(taskFlexGridSizer);

    /* ---Controls--- */
    /* Project Name Control */
    auto projectName = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Name"));
    taskFlexGridSizer->Add(projectName, common::sizers::ControlCenterVertical);

    pNameCtrl = new wxTextCtrl(projectDetailsPanel,
        wxID_STATIC,
        wxGetEmptyString(),
        wxDefaultPosition,
        wxSize(150, -1),
        wxTE_LEFT,
        wxDefaultValidator,
        wxT("project_name_ctrl"));
    pNameCtrl->SetToolTip(wxT("Enter a name for the project"));
    taskFlexGridSizer->Add(pNameCtrl, common::sizers::ControlDefault);

    /* Project Display Name Control */
    auto projectDisplayName = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Display Name"));
    taskFlexGridSizer->Add(projectDisplayName, common::sizers::ControlCenterVertical);

    pDisplayNameCtrl = new wxTextCtrl(projectDetailsPanel,
        wxID_STATIC,
        wxGetEmptyString(),
        wxDefaultPosition,
        wxSize(150, -1),
        wxTE_LEFT,
        wxDefaultValidator,
        wxT("project_display_name_ctrl"));
    pDisplayNameCtrl->SetToolTip(wxT("Enter a shortened, convenient display name for the project"));
    taskFlexGridSizer->Add(pDisplayNameCtrl, common::sizers::ControlDefault);

    /* Employer Choice Ctrl */
    auto employerText = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Employer"));
    taskFlexGridSizer->Add(employerText, common::sizers::ControlCenterVertical);

    pEmployerChoiceCtrl = new wxChoice(projectDetailsPanel, IDC_EMPLOYERCHOICE, wxDefaultPosition, wxSize(150, -1));
    pEmployerChoiceCtrl->SetToolTip(wxT("Select a employer to associate the project with"));
    pEmployerChoiceCtrl->AppendString(wxT("Select a employer"));
    pEmployerChoiceCtrl->SetSelection(0);
    taskFlexGridSizer->Add(pEmployerChoiceCtrl, common::sizers::ControlDefault);

    /* Client Choice Ctrl */
    auto clientText = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT("Client"));
    taskFlexGridSizer->Add(clientText, common::sizers::ControlCenterVertical);

    pClientChoiceCtrl = new wxChoice(projectDetailsPanel, IDC_CLIENTCHOICE, wxDefaultPosition, wxSize(150, -1));
    pClientChoiceCtrl->SetToolTip(wxT("Select a client to associate this project with"));
    pClientChoiceCtrl->AppendString(wxT("Select a client"));
    pClientChoiceCtrl->SetSelection(0);
    pClientChoiceCtrl->Disable();
    taskFlexGridSizer->Add(pClientChoiceCtrl, common::sizers::ControlDefault);

    if (bIsEdit) {
        auto isActiveFiller = new wxStaticText(projectDetailsPanel, wxID_STATIC, wxT(""));
        taskFlexGridSizer->Add(isActiveFiller, common::sizers::ControlDefault);

        /* Is Active Checkbox Control */
        pIsActiveCtrl = new wxCheckBox(projectDetailsPanel, IDC_ISACTIVE, wxT("Is Active"));
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
    auto separation_line = new wxStaticLine(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, wxT("project_details_static_line"));
    mainSizer->Add(separation_line, 0, wxEXPAND | wxALL, 1);

    /* Button Panel */
    auto buttonPanel = new wxPanel(this, wxID_STATIC);
    auto buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);
    mainSizer->Add(buttonPanel, common::sizers::ControlCenter);

    auto okButton = new wxButton(buttonPanel, ids::ID_SAVE, wxT("&Save"));
    auto cancelButton = new wxButton(buttonPanel, wxID_CANCEL, wxT("&Cancel"));

    buttonPanelSizer->Add(okButton, common::sizers::ControlDefault);
    buttonPanelSizer->Add(cancelButton, common::sizers::ControlDefault);
}

void ProjectDialog::FillControls()
{
    std::vector<models::employer> employers;
    services::db_service dbService;
    try {
        employers = dbService.get_employers();
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_employers() - {0:d} : {1}", e.get_code(), e.what());
    }

    for (auto employer : employers) {
        pEmployerChoiceCtrl->Append(employer.employer_name, util::IntToVoidPointer(employer.employer_id));
    }
}

void ProjectDialog::DataToControls()
{
    services::db_service dbService;
    models::project project;

    try {
        project = dbService.get_project_by_id(mProjectId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_project_by_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    pNameCtrl->SetValue(project.project_name);
    pDisplayNameCtrl->SetValue(project.display_name);
    pEmployerChoiceCtrl->SetStringSelection(project.employer_name);

    if (!project.client_name.empty()) {
        pClientChoiceCtrl->Clear();
        pClientChoiceCtrl->AppendString(wxT("Select a client"));
        std::vector<models::client> clients;
        try {
            services::db_service clientService;
            clients = clientService.get_clients_by_employer_id(project.employer_id);
        } catch (const sqlite::sqlite_exception& e) {
            pLogger->error("Error occured in get_clients_by_employer_id() - {0:d} : {1}", e.get_code(), e.what());
        }

        for (auto client : clients) {
            pClientChoiceCtrl->Append(client.client_name, util::IntToVoidPointer(client.client_id));
        }

        pClientChoiceCtrl->SetSelection(0);
        if (!pClientChoiceCtrl->IsEnabled()) {
            pClientChoiceCtrl->Enable();
        }
        pClientChoiceCtrl->SetStringSelection(project.client_name);
    }

    wxString dateCreatedString = util::ConvertUnixTimestampToString(project.date_created_utc);
    wxString dateCreatedLabel = pDateCreatedTextCtrl->GetLabelText();
    pDateCreatedTextCtrl->SetLabel(wxString::Format(dateCreatedLabel, dateCreatedString));

    wxString dateUpdatedString = util::ConvertUnixTimestampToString(project.date_modified_utc);
    wxString dateUpdatedLabel = pDateUpdatedTextCtrl->GetLabelText();
    pDateUpdatedTextCtrl->SetLabel(wxString::Format(dateUpdatedLabel, dateUpdatedString));

    pIsActiveCtrl->SetValue(project.is_active);
}

bool ProjectDialog::Validate()
{
    if (mNameText.empty()) {
        auto message = wxString::Format(Constants::Messages::IsEmpty, wxT("Project name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mNameText.length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Project name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mNameText.length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Project name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDisplayNameText.empty()) {
        auto message = wxString::Format(Constants::Messages::IsEmpty, wxT("Display name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDisplayNameText.length() < 2) {
        auto message = wxString::Format(Constants::Messages::TooShort, wxT("Display name"));
        wxMessageBox(message, wxT("Validation failure"), wxOK | wxICON_EXCLAMATION);
        return false;
    }

    if (mDisplayNameText.length() > 255) {
        auto message = wxString::Format(Constants::Messages::TooLong, wxT("Display name"));
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

bool ProjectDialog::AreControlsEmpty()
{
    bool isEmpty = mNameText.empty() && mDisplayNameText.empty() && (mEmployerId == -1 || mEmployerId == 0);
    return isEmpty;
}

void ProjectDialog::OnEmployerSelect(wxCommandEvent& event)
{
    pClientChoiceCtrl->Clear();
    pClientChoiceCtrl->AppendString(wxT("Select a client"));
    int employerId = util::VoidPointerToInt(event.GetClientData());
    std::vector<models::client> clients;
    try {
        services::db_service clientService;
        clients = clientService.get_clients_by_employer_id(employerId);
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in get_clients_by_employer_id() - {0:d} : {1}", e.get_code(), e.what());
    }

    pClientChoiceCtrl->SetSelection(0);
    if (!clients.empty()) {
        for (auto client : clients) {
            pClientChoiceCtrl->Append(client.client_name, util::IntToVoidPointer(client.client_id));
        }

        if (!pClientChoiceCtrl->IsEnabled()) {
            pClientChoiceCtrl->Enable();
        }
    }
}

void ProjectDialog::OnSave(wxCommandEvent& event)
{
    mNameText = pNameCtrl->GetValue();
    mDisplayNameText = pDisplayNameCtrl->GetValue();

    mEmployerId = util::VoidPointerToInt(pEmployerChoiceCtrl->GetClientData(pEmployerChoiceCtrl->GetSelection()));
    mClientId = util::VoidPointerToInt(pClientChoiceCtrl->GetClientData(pClientChoiceCtrl->GetSelection()));

    bool isValid = Validate();
    if (!isValid) {
        return;
    }

    try {
        services::db_service dbService;
        if (bIsEdit && pIsActiveCtrl->IsChecked()) {
            models::project project;
            project.project_name = std::string(mNameText.ToUTF8());
            project.display_name = std::string(mDisplayNameText.ToUTF8());
            project.employer_id = mEmployerId;
            if (mClientId == -1 || mClientId == 0) {
                project.client_id = 0;
            } else {
                project.client_id = mClientId;
            }
            dbService.update_project(project);
        }
        if (bIsEdit && !pIsActiveCtrl->IsChecked()) {
            dbService.delete_project(mProjectId, util::UnixTimestamp());
        }
        if (!bIsEdit) {
            if (mClientId == -1 || mClientId == 0) {
                dbService.create_new_project(
                    std::string(mNameText.ToUTF8()), std::string(mDisplayNameText.ToUTF8()), mEmployerId, nullptr);
            } else {
                dbService.create_new_project(
                    std::string(mNameText.ToUTF8()), std::string(mDisplayNameText.ToUTF8()), mEmployerId, &mClientId);
            }
        }
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in project OnSave() - {0:d} : {1}", e.get_code(), e.what());
    }

    EndModal(ids::ID_SAVE);
}

void ProjectDialog::OnCancel(wxCommandEvent& event)
{
    bool areControlsEmpty = AreControlsEmpty();
    if (!areControlsEmpty) {
        int answer = wxMessageBox(wxT("Are you sure you want to cancel?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
        if (answer == wxYES) {
            EndModal(wxID_CANCEL);
        }
    } else {
        EndModal(wxID_CANCEL);
    }
}
void ProjectDialog::OnIsActiveCheck(wxCommandEvent& event)
{
    if (event.IsChecked()) {
        pNameCtrl->Enable();
        pDisplayNameCtrl->Enable();
        pEmployerChoiceCtrl->Enable();
        pClientChoiceCtrl->Enable();
    } else {
        pNameCtrl->Disable();
        pDisplayNameCtrl->Disable();
        pEmployerChoiceCtrl->Disable();
        pClientChoiceCtrl->Disable();
    }
}
} // namespace app::dialog
